static void
ndps_defrag(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    int                 len=0;
    tvbuff_t            *next_tvb = NULL;
    fragment_data       *fd_head;
    spx_info            *spx_info;
    ndps_req_hash_value	*request_value = NULL;
    conversation_t      *conversation;

    /
    spx_info = pinfo->private_data;
    /
    if (!ndps_defragment) {
        dissect_ndps(tvb, pinfo, tree);
        return;
    }
    /
    if (!pinfo->fd->flags.visited) 
    {
        /
        conversation = find_conversation(pinfo->fd->num, &pinfo->src, &pinfo->dst,
            PT_NCP, (guint32) pinfo->srcport, (guint32) pinfo->srcport, 0);
    
        if (conversation == NULL) 
        {
            /
            conversation = conversation_new(pinfo->fd->num, &pinfo->src, &pinfo->dst,
                PT_NCP, (guint32) pinfo->srcport, (guint32) pinfo->srcport, 0);
            /
            request_value = ndps_hash_insert(conversation, (guint32) pinfo->srcport);
        }
        /
        request_value = ndps_hash_lookup(conversation, (guint32) pinfo->srcport);
        if (request_value == NULL) 
        {
            /
            request_value = ndps_hash_insert(conversation, (guint32) pinfo->srcport);
        }
        /
        p_add_proto_data(pinfo->fd, proto_ndps, (void*) request_value);
    }
    else
    {
        /
        request_value = p_get_proto_data(pinfo->fd, proto_ndps);
    }
    if (!request_value)
    {
        /
        dissect_ndps(tvb, pinfo, tree);
        return;
    }
    /
    if (!spx_info->eom) {
        request_value->ndps_frag = TRUE;
    }
    /
    if (request_value->ndps_frag || (request_value->ndps_end_frag == pinfo->fd->num)) 
    {
        /
        tid = (pinfo->srcport+pinfo->destport);
        len = tvb_reported_length_remaining(tvb, 0);
        if (tvb_bytes_exist(tvb, 0, len))
        {
            fd_head = fragment_add_seq_next(tvb, 0, pinfo, tid, ndps_fragment_table, ndps_reassembled_table, len, !spx_info->eom);
            if (fd_head != NULL) 
            {
                /
                if (fd_head->next != NULL && spx_info->eom) 
                {
                    proto_item *frag_tree_item;

                    next_tvb = tvb_new_real_data(fd_head->data,
                        fd_head->len, fd_head->len);
                    tvb_set_child_real_data_tvbuff(tvb,
                        next_tvb);
                    add_new_data_source(pinfo,
                        next_tvb,
                        "Reassembled NDPS");
                    /
                    if (tree) 
                    {
                        show_fragment_seq_tree(fd_head,
                            &ndps_frag_items,
                            tree, pinfo,
                            next_tvb, &frag_tree_item);
                        tid++;
                    }
                    /
                    request_value->ndps_end_frag = pinfo->fd->num;

                } 
                else 
                {
                    /
                    next_tvb = tvb_new_subset(tvb, 0, -1, -1);
                    if (check_col(pinfo->cinfo, COL_INFO))
                    {
                      if (!spx_info->eom)
                      {
                        col_append_fstr(pinfo->cinfo, COL_INFO, "[NDPS Fragment]");
                      }
                    }
                }
            }
            else 
            {
                /
                if (check_col(pinfo->cinfo, COL_INFO))
                {
                  if (!spx_info->eom)
                  {
                    col_append_fstr(pinfo->cinfo, COL_INFO, "[NDPS Fragment]");
                  }
                }
                next_tvb = NULL;
            }
        }
        else 
        {
            /
            next_tvb = tvb_new_subset(tvb, 0, -1, -1);
        }
        if (next_tvb == NULL)
        {
            /
            next_tvb = tvb_new_subset (tvb, 0, -1, -1);
            call_dissector(ndps_data_handle, next_tvb, pinfo, tree);
        }
        else
        {
            /
            if (spx_info->eom) {
                request_value->ndps_frag = FALSE;
                dissect_ndps(next_tvb, pinfo, tree);
            }
        }
    }
    else
    {
        /
        request_value->ndps_frag = FALSE;
        dissect_ndps(tvb, pinfo, tree);
    }
}
