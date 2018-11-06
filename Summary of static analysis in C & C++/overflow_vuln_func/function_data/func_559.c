static int
dissect_ndps_request(tvbuff_t *tvb, packet_info *pinfo, proto_tree *ndps_tree, guint32 ndps_prog, guint32 ndps_func, int foffset)
{
    ndps_req_hash_value *request_value = NULL;
    conversation_t      *conversation;
    guint32             ii;
    guint32             jj;
    guint32             field_len;
    guint32             cred_type;
    guint32             resource_type;
    guint32             filter_type;
    guint32             print_type;
    guint32             length;
    guint32             number_of_items;
    guint32             number_of_items2;
    guint32             doc_content;
    guint32             list_attr_op;
    guint32             scope;
    guint32             job_type;
    gboolean            supplier_flag;
    gboolean            language_flag;
    gboolean            method_flag;
    gboolean            delivery_address_flag;
    guint32             profiles_type;
    guint32             profiles_choice_type;
    guint32             integer_type_flag;
    guint32             local_servers_type;
    gint                length_remaining;
    proto_tree          *atree, *btree, *ctree, *dtree;
    proto_item          *aitem, *bitem, *citem, *ditem, *expert_item;

    if (!pinfo->fd->flags.visited)
    {
        /

        conversation = find_conversation(pinfo->num, &pinfo->src, &pinfo->dst,
            PT_NCP, (guint32) pinfo->srcport, (guint32) pinfo->srcport, 0);

        if (conversation == NULL)
        {
            /
            conversation = conversation_new(pinfo->num, &pinfo->src, &pinfo->dst,
                PT_NCP, (guint32) pinfo->srcport, (guint32) pinfo->srcport, 0);
        }

        request_value = ndps_hash_insert(conversation, (guint32) pinfo->srcport);
        request_value->ndps_prog = ndps_prog;
        request_value->ndps_func = ndps_func;
        request_value->ndps_frame_num = pinfo->num;
    }
    switch(ndps_prog)
    {
    case 0x060976:  /
        switch(ndps_func)
        {
        case 0x00000001:    /
            foffset = credentials(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000002:    /
            foffset = credentials(tvb, pinfo, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_retrieve_restrictions, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            number_of_items=tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_bind_security_option_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Security %u", ii+1);
                length = tvb_get_ntohl(tvb, foffset);
                if (length==4)
                {
                    proto_tree_add_uint(atree, hf_bind_security, tvb, foffset, 4, length);
                }
                foffset += 4;
                proto_item_set_end(aitem, tvb, foffset);
            }
            foffset = qualifiedname(tvb, ndps_tree, foffset);
            break;
        case 0x00000003:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset,
            4, ENC_BIG_ENDIAN);
            break;
        case 0x00000004:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            print_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_print_arg, tvb, foffset, 4, print_type);
            foffset += 4;
            switch (print_type)
            {
            case 0:     /
                foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL);
                proto_tree_add_item(ndps_tree, hf_sub_complete, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Transfer Method");
                number_of_items = tvb_get_ntohl(tvb, foffset);
                expert_item = proto_tree_add_uint(atree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (ii = 0; ii < number_of_items; ii++ )
                {
                    if (ii >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                        break;
                    }
                    btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Attribute %u", ii+1);
                    foffset = objectidentifier(tvb, btree, foffset);
                    number_of_items2 = tvb_get_ntohl(tvb, foffset);
                    expert_item = proto_tree_add_uint(btree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items2);
                    foffset += 4;
                    for (jj = 0; jj < number_of_items2; jj++ )
                    {
                        if (jj >= NDPS_MAX_ITEMS) {
                            expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                            break;
                        }
                        ctree = proto_tree_add_subtree_format(btree, tvb, foffset, -1, ett_ndps, &citem, "Value %u", jj+1);
                        foffset = attribute_value(tvb, pinfo, ctree, foffset);
                        proto_item_set_end(citem, tvb, foffset);
                    }
                    proto_tree_add_item(btree, hf_ndps_qualifier, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                    proto_item_set_end(bitem, tvb, foffset);
                }
                proto_item_set_end(aitem, tvb, foffset);
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Document Content");
                number_of_items = tvb_get_ntohl(tvb, foffset);
                expert_item = proto_tree_add_uint(atree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (ii = 0; ii < number_of_items; ii++ )
                {
                    if (ii >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                        break;
                    }
                    btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Type %u", ii+1);
                    foffset = objectidentifier(tvb, btree, foffset);
                    proto_item_set_end(bitem, tvb, foffset);
                }
                foffset += align_4(tvb, foffset);
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items);
                foffset += 4;
                doc_content = tvb_get_ntohl(tvb, foffset);
                expert_item = proto_tree_add_uint(atree, hf_doc_content, tvb, foffset, 4, doc_content);
                foffset += 4;
                for (ii = 0; ii < number_of_items; ii++ )
                {
                    if (ii >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                        break;
                    }
                    btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Value %u", ii+1);
                    if (doc_content==0)
                    {
                        length = tvb_get_ntohl(tvb, foffset);
                        proto_tree_add_uint(btree, hf_ndps_included_doc_len, tvb, foffset, 4, length);
                        foffset += 4;
                        length_remaining = tvb_reported_length_remaining(tvb, foffset);
                        if (length_remaining == -1 || length > (guint32) length_remaining) /
                        {
                            proto_tree_add_item(btree, hf_ndps_data, tvb, foffset, -1, ENC_NA);
                            return foffset;
                        }
                        if (length==4)
                        {
                            proto_tree_add_item(btree, hf_ndps_included_doc, tvb, foffset, length, ENC_NA);
                        }
                        foffset += length;
                        foffset += (length%2);
                        if ((int) foffset <= 0)
                            THROW(ReportedBoundsError);
                    }
                    else
                    {
                        foffset = ndps_string(tvb, hf_ndps_ref_name, btree, foffset, NULL);
                        foffset = name_or_id(tvb, btree, foffset);
                    }
                    proto_item_set_end(bitem, tvb, foffset);
                }
                proto_item_set_end(aitem, tvb, foffset);
                foffset += 4;
                if (align_4(tvb, foffset)>0) {
                    foffset += align_4(tvb, foffset);
                }
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Document Type");
                number_of_items = tvb_get_ntohl(tvb, foffset);
                expert_item = proto_tree_add_uint(atree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (ii = 0; ii < number_of_items; ii++ )
                {
                    if (ii >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                        break;
                    }
                    btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Attribute %u", ii+1);
                    foffset = objectidentifier(tvb, btree, foffset);
                    number_of_items2 = tvb_get_ntohl(tvb, foffset);
                    expert_item = proto_tree_add_uint(btree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items2);
                    foffset += 4;
                    for (jj = 0; jj < number_of_items2; jj++ )
                    {
                        if (jj >= NDPS_MAX_ITEMS) {
                            expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                            break;
                        }
                        ctree = proto_tree_add_subtree_format(btree, tvb, foffset, -1, ett_ndps, &citem, "Value %u", jj+1);
                        foffset = attribute_value(tvb, pinfo, ctree, foffset);
                        proto_item_set_end(citem, tvb, foffset);
                    }
                    proto_tree_add_item(btree, hf_ndps_qualifier, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                    proto_item_set_end(bitem, tvb, foffset);
                }
                proto_item_set_end(aitem, tvb, foffset);
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Document Attributes");
                number_of_items = tvb_get_ntohl(tvb, foffset);
                expert_item = proto_tree_add_uint(atree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (ii = 0; ii < number_of_items; ii++ )
                {
                    if (ii >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                        break;
                    }
                    btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Attribute %u", ii+1);
                    foffset = attribute_value(tvb, pinfo, btree, foffset);  /
                    proto_item_set_end(bitem, tvb, foffset);
                }
                break;
            case 1:     /
                foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL);
                proto_tree_add_item(ndps_tree, hf_local_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_tree_add_item(ndps_tree, hf_sub_complete, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Transfer Method");
                number_of_items = tvb_get_ntohl(tvb, foffset);
                expert_item = proto_tree_add_uint(atree, hf_ndps_num_transfer_methods, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (ii = 0; ii < number_of_items; ii++ )
                {
                    if (ii >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                        break;
                    }
                    btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Method %u", ii+1);
                    foffset = objectidentifier(tvb, btree, foffset); /
                    proto_item_set_end(bitem, tvb, foffset);
                }
                proto_tree_add_item(ndps_tree, hf_doc_content, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_item_set_end(aitem, tvb, foffset);
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Document Type");
                number_of_items = tvb_get_ntohl(tvb, foffset);
                expert_item = proto_tree_add_uint(atree, hf_ndps_num_doc_types, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (ii = 0; ii < number_of_items; ii++ )
                {
                    if (ii >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                        break;
                    }
                    btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Type %u", ii+1);
                    foffset = objectidentifier(tvb, btree, foffset); /
                    proto_item_set_end(bitem, tvb, foffset);
                }
                foffset += align_4(tvb, foffset);
                proto_item_set_end(aitem, tvb, foffset);
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Document Attributes");
                number_of_items = tvb_get_ntohl(tvb, foffset);
                expert_item = proto_tree_add_uint(atree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (ii = 0; ii < number_of_items; ii++ )
                {
                    if (ii >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                        break;
                    }
                    btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Attribute %u", ii+1);
                    foffset = attribute_value(tvb, pinfo, btree, foffset);  /
                    proto_item_set_end(bitem, tvb, foffset);
                }
                proto_item_set_end(aitem, tvb, foffset);
                break;
            case 2:     /
                foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL);
                proto_tree_add_item(ndps_tree, hf_local_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                break;
            default:
                break;
            }
            break;
        case 0x00000005:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL);
            proto_tree_add_item(ndps_tree, hf_local_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_document_number, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Job Modifications");
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(atree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Modification %u", ii+1);
                foffset = attribute_value(tvb, pinfo, btree, foffset);  /
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Document Modifications");
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(atree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Modification %u", ii+1);
                foffset = attribute_value(tvb, pinfo, btree, foffset);  /
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            break;
        case 0x00000006:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL);
            proto_tree_add_item(ndps_tree, hf_local_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_document_number, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Cancel Message");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Retention Period");
            proto_tree_add_item(atree, hf_ndps_status_flags, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(atree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_item_set_end(aitem, tvb, foffset);
            break;
        case 0x00000007:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            list_attr_op = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_attrs_arg, tvb, foffset, 4, list_attr_op);
            foffset += 4;
            if (list_attr_op==0) /
            {
                length = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_context_len, tvb, foffset, 4, length);
                foffset += 4;
                if (length!=0)
                {
                    proto_tree_add_item(ndps_tree, hf_ndps_context, tvb, foffset, length, ENC_NA);
                }
                foffset += length;
                foffset += (length%2);
                proto_tree_add_item(ndps_tree, hf_ndps_abort_flag, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                number_of_items = tvb_get_ntohl(tvb, foffset);
                expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (ii = 0; ii < number_of_items; ii++ )
                {
                    if (ii >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                        break;
                    }
                    atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Attribute %u", ii+1);
                    foffset = attribute_value(tvb, pinfo, atree, foffset);
                    proto_item_set_end(aitem, tvb, foffset);
                }
            }
            else                                  /
            {
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Object Class");
                foffset = objectidentifier(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
                foffset += 4;
                foffset += align_4(tvb, foffset);
                scope = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_scope, tvb, foffset, 4, scope);
                foffset += 4;
                if (scope!=0)    /
                {
                    atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Selector Option");
                    number_of_items = tvb_get_ntohl(tvb, foffset); /
                    expert_item = proto_tree_add_uint(atree, hf_ndps_num_options, tvb, foffset, 4, number_of_items);
                    foffset += 4;
                    for (ii = 0; ii < number_of_items; ii++ )
                    {
                        if (ii >= NDPS_MAX_ITEMS) {
                            expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                            break;
                        }
                        btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Option %u", ii+1);
                        foffset = objectidentification(tvb, btree, foffset);
                        proto_item_set_end(bitem, tvb, foffset);
                    }
                    proto_item_set_end(aitem, tvb, foffset);
                    foffset += align_4(tvb, foffset);
                    filter_type = tvb_get_ntohl(tvb, foffset);
                    proto_tree_add_uint(ndps_tree, hf_ndps_filter, tvb, foffset, 4, filter_type);
                    foffset += 4;
#if 0
                    if (filter_type == 0 || filter_type == 3 )
                    {
                        foffset = filteritem(tvb, ndps_tree, foffset);
                    }
                    else
                    {
                        atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Filter Items");
                        number_of_items = tvb_get_ntohl(tvb, foffset);
                        expert_item = proto_tree_add_uint(atree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
                        foffset += 4;
                        for (ii = 0; ii < number_of_items; ii++ )
                        {
                            if (ii >= NDPS_MAX_ITEMS) {
                                expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                                break;
                            }
                            foffset = filteritem(tvb, ndps_tree, foffset);
                        }
                        proto_item_set_end(aitem, tvb, foffset);
                    }
#endif
                    proto_tree_add_item(ndps_tree, hf_ndps_time_limit, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                    proto_tree_add_item(ndps_tree, hf_ndps_count_limit, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4; /
                }
                foffset += 4;   /
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Requested Attributes");
                number_of_items = tvb_get_ntohl(tvb, foffset); /
                expert_item = proto_tree_add_uint(atree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (ii = 0; ii < number_of_items; ii++ )
                {
                    if (ii >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                        break;
                    }
                    btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Attribute %u", ii+1);
                    foffset = objectidentifier(tvb, btree, foffset);
                    proto_item_set_end(bitem, tvb, foffset);
                }
                proto_item_set_end(aitem, tvb, foffset); /
                if (number_of_items == 0)
                {
                    break;
                }
                proto_tree_add_item(ndps_tree, hf_ndps_operator, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            }
            break;
        case 0x00000008:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Job ID");
            foffset = ndps_string(tvb, hf_ndps_pa_name, atree, foffset, NULL);
            proto_tree_add_item(atree, hf_local_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_item_set_end(aitem, tvb, foffset);
            /
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Message Option");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000009:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            job_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_interrupt_job_type, tvb, foffset, 4, job_type);
            foffset += 4;
            if (job_type==0)
            {
                /
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Job ID");
                foffset = ndps_string(tvb, hf_ndps_pa_name, atree, foffset, NULL);
                proto_tree_add_item(atree, hf_local_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_item_set_end(aitem, tvb, foffset);
                /
            }
            else
            {
                foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL);
            }
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Interrupt Message Option");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Interrupting Job");
            foffset = ndps_string(tvb, hf_ndps_pa_name, atree, foffset, NULL);
            proto_tree_add_item(atree, hf_local_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x0000000a:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            job_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_pause_job_type, tvb, foffset, 4, job_type);
            foffset += 4;
            if (job_type==0)
            {
                /
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Job ID");
                foffset = ndps_string(tvb, hf_ndps_pa_name, atree, foffset, NULL);
                proto_tree_add_item(atree, hf_local_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_item_set_end(aitem, tvb, foffset);
                /
            }
            else
            {
                foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL);
            }
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Pause Message Option");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x0000000b:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Job ID");
            foffset = ndps_string(tvb, hf_ndps_pa_name, atree, foffset, NULL);
            proto_tree_add_item(atree, hf_local_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_item_set_end(aitem, tvb, foffset);
            /
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Resume Message Option");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x0000000c:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Clean Message Option");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x0000000d:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Object Class");
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Object ID");
            foffset = objectidentification(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_force, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Reference Object Option");
            foffset = objectidentification(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Object Attribute");
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(atree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Attribute %u", ii+1);
                foffset = attribute_value(tvb, pinfo, btree, foffset);  /
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x0000000e:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Object Class");
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Object ID");
            foffset = objectidentification(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x0000000f:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Disable PA Message Option");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000010:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Enable PA Message Option");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000011:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = qualifiedname(tvb, ndps_tree, foffset);
            foffset = address_item(tvb, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_resubmit_op_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Resubmit Job");
            number_of_items = tvb_get_ntohl(tvb, foffset); /
            expert_item = proto_tree_add_uint(atree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                /
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Job ID");
                foffset = ndps_string(tvb, hf_ndps_pa_name, btree, foffset, NULL);
                proto_tree_add_item(btree, hf_local_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_item_set_end(bitem, tvb, foffset);
                /
                proto_tree_add_item(atree, hf_ndps_document_number, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                /
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Job Attributes");
                number_of_items2 = tvb_get_ntohl(tvb, foffset);
                expert_item = proto_tree_add_uint(btree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items2);
                foffset += 4;
                for (jj = 0; jj < number_of_items2; jj++ )
                {
                    if (jj >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                        break;
                    }
                    ctree = proto_tree_add_subtree_format(btree, tvb, foffset, -1, ett_ndps, &citem, "Attribute %u", jj+1);
                    foffset = attribute_value(tvb, pinfo, ctree, foffset);  /
                    proto_item_set_end(citem, tvb, foffset);
                }
                proto_item_set_end(bitem, tvb, foffset);
                /
                /
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Document Attributes");
                number_of_items2 = tvb_get_ntohl(tvb, foffset);
                expert_item = proto_tree_add_uint(btree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items2);
                foffset += 4;
                for (jj = 0; jj < number_of_items2; jj++ )
                {
                    if (jj >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                        break;
                    }
                    ctree = proto_tree_add_subtree_format(btree, tvb, foffset, -1, ett_ndps, &citem, "Attribute %u", jj+1);
                    foffset = attribute_value(tvb, pinfo, ctree, foffset);  /
                    proto_item_set_end(citem, tvb, foffset);
                }
                proto_item_set_end(bitem, tvb, foffset);
                /
            }
            proto_item_set_end(aitem, tvb, foffset);   /
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Resubmit Message Option");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000012:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Object Class");
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Object ID");
            foffset = objectidentification(tvb, atree, foffset);
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Attribute Modifications");
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(atree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Modification `%d", ii+1);
                foffset = attribute_value(tvb, pinfo, btree, foffset);  /
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000013:    /
        case 0x0000001e:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_shutdown_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL);
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Shutdown Message Option");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000014:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL);
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Startup Message Option");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000015:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Job Identification");
            foffset = ndps_string(tvb, hf_ndps_pa_name, atree, foffset, NULL);
            proto_tree_add_item(atree, hf_local_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_item_set_end(aitem, tvb, foffset);
            /
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Reference Job ID");
            foffset = ndps_string(tvb, hf_ndps_pa_name, atree, foffset, NULL);
            proto_tree_add_item(atree, hf_local_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000016:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Pause Message Option");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000017:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Resume Message Option");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000018:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_get_status_flag, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_data, tvb, foffset+4, tvb_get_ntohl(tvb, foffset), ENC_NA);
            break;
        case 0x00000019:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Operation ID");
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x0000001a:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_persistence, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = qualifiedname(tvb, ndps_tree, foffset);
            foffset = ndps_string(tvb, hf_ndps_supplier_name, ndps_tree, foffset, NULL);
            proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Method ID");
            foffset = name_or_id(tvb, atree, foffset);
            foffset += align_4(tvb, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Delivery Address");
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(atree, hf_ndps_delivery_add_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Address %u", ii+1);
                foffset = address_item(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            foffset = event_object_set(tvb, pinfo, ndps_tree, foffset);
            foffset = qualifiedname(tvb, ndps_tree, foffset);
            break;
        case 0x0000001b:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 0x0000001c:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item_ret_uint(ndps_tree, hf_ndps_supplier_flag, tvb, foffset, 4, ENC_BIG_ENDIAN, &supplier_flag);
            foffset += 4;
            if (supplier_flag)
            {
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Supplier ID");
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(atree, hf_ndps_attribute_value, tvb, foffset, length, ENC_BIG_ENDIAN);
                }
                tvb_ensure_bytes_exist(tvb, foffset, length);
                foffset += length;
                proto_item_set_end(aitem, tvb, foffset);
            }
            proto_tree_add_item_ret_uint(ndps_tree, hf_ndps_language_flag, tvb, foffset, 4, ENC_BIG_ENDIAN, &language_flag);
            foffset += 4;
            if (language_flag)
            {
                proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
            }
            proto_tree_add_item_ret_uint(ndps_tree, hf_ndps_method_flag, tvb, foffset, 4, ENC_BIG_ENDIAN, &method_flag);
            foffset += 4;
            if (method_flag)
            {
                /
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Method ID");
                foffset = name_or_id(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
                /
            }
            proto_tree_add_item_ret_uint(ndps_tree, hf_ndps_delivery_address_flag, tvb, foffset, 4, ENC_BIG_ENDIAN, &delivery_address_flag);
            foffset += 4;
            if (delivery_address_flag)
            {
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Delivery Address");
                foffset = print_address(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
            }
            foffset = event_object_set(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x0000001d:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            profiles_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_list_profiles_type, tvb, foffset, 4, profiles_type);
            foffset += 4;
            if (profiles_type==0)   /
            {
                foffset = qualifiedname(tvb, ndps_tree, foffset);
                profiles_choice_type = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_list_profiles_choice_type, tvb, foffset, 4, profiles_choice_type);
                foffset += 4;
                if (profiles_choice_type==0)   /
                {
                    foffset = cardinal_seq(tvb, pinfo, ndps_tree, foffset);
                }
                else
                {
                    atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Consumer");
                    foffset = qualifiedname(tvb, atree, foffset);
                    proto_item_set_end(aitem, tvb, foffset);
                    /
                    atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Method ID");
                    foffset = name_or_id(tvb, atree, foffset);
                    /
                    proto_tree_add_item(atree, hf_ndps_language_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                    proto_item_set_end(aitem, tvb, foffset);
                }
                proto_tree_add_item(ndps_tree, hf_ndps_list_profiles_result_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                /
                integer_type_flag = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_integer_type_flag, tvb, foffset, 4, integer_type_flag);
                foffset += 4;
                if (integer_type_flag!=0)
                {
                    proto_tree_add_item(ndps_tree, hf_ndps_integer_type_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                }
                /
            }
            else                                    /
            {
                length = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_context_len, tvb, foffset, 4, length);
                foffset += 4;
                if (length!=0)
                {
                    proto_tree_add_item(ndps_tree, hf_ndps_context, tvb, foffset, length, ENC_NA);
                }
                foffset += length;
                foffset += (length%2);
                proto_tree_add_item(ndps_tree, hf_ndps_abort_flag, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
            }
            break;
        case 0x0000001f:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Cancel Shutdown Message Option");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000020:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_ds_info_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = ndps_string(tvb, hf_ndps_printer_name, ndps_tree, foffset, NULL);
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "DS Object Name");
            foffset = qualifiedname(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            break;
        case 0x00000021:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Clean Message Option");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000022:    /
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length!=0)
            {
                proto_tree_add_item(ndps_tree, hf_ndps_guid, tvb, foffset, length, ENC_NA);
            }
            foffset += length;
            break;
        case 0x00000023:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_persistence, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Consumer Name");
            foffset = qualifiedname(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            foffset = ndps_string(tvb, hf_ndps_supplier_name, ndps_tree, foffset, NULL);
            proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Method ID");
            foffset = name_or_id(tvb, atree, foffset);
            foffset += align_4(tvb, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Delivery Address");
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(atree, hf_ndps_delivery_add_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Address %u", ii+1);
                foffset = address_item(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            foffset = event_object_set(tvb, pinfo, ndps_tree, foffset);
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Account");
            foffset = qualifiedname(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Notify Attributes");
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(atree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Attribute %u", ii+1);
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            /
            proto_tree_add_item(ndps_tree, hf_notify_time_interval, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_notify_sequence_number, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_notify_lease_exp_time, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = ndps_string(tvb, hf_notify_printer_uri, ndps_tree, foffset, NULL);
            /
            break;
        case 0x00000024:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            profiles_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_list_profiles_type, tvb, foffset, 4, profiles_type);
            foffset += 4;
            if (profiles_type==0)   /
            {
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Supplier Alias");
                foffset = qualifiedname(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
                profiles_choice_type = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_list_profiles_choice_type, tvb, foffset, 4, profiles_choice_type);
                foffset += 4;
                if (profiles_choice_type==0)   /
                {
                    foffset = cardinal_seq(tvb, pinfo, ndps_tree, foffset);
                }
                else
                {
                    atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Consumer");
                    foffset = qualifiedname(tvb, atree, foffset);
                    proto_item_set_end(aitem, tvb, foffset);
                    /
                    atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Method ID");
                    foffset = name_or_id(tvb, atree, foffset);
                    proto_item_set_end(aitem, tvb, foffset);
                    /
                    proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                }
                proto_tree_add_item(ndps_tree, hf_ndps_list_profiles_result_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                /
                integer_type_flag = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_integer_type_flag, tvb, foffset, 4, integer_type_flag);
                foffset += 4;
                if (integer_type_flag!=0)
                {
                    proto_tree_add_item(ndps_tree, hf_ndps_integer_type_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                }
                /
            }
            else                                    /
            {
                length = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_context_len, tvb, foffset, 4, length);
                foffset += 4;
                if (length!=0)
                {
                    proto_tree_add_item(ndps_tree, hf_ndps_context, tvb, foffset, length, ENC_NA);
                }
                foffset += length;
                foffset += (length%2);
                proto_tree_add_item(ndps_tree, hf_ndps_abort_flag, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
            }
            break;
        default:
            break;
        }
        break;
    case 0x060977:  /
        switch(ndps_func)
        {
        case 0x00000001:    /
            foffset = credentials(tvb, pinfo, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_retrieve_restrictions, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            number_of_items=tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_bind_security_option_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Security %u", ii+1);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(atree, hf_bind_security, tvb, foffset, length, ENC_BIG_ENDIAN);
                }
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 0x00000002:    /
            break;
        case 0x00000003:    /
            proto_tree_add_item(ndps_tree, hf_ndps_list_services_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 0x00000004:    /
            proto_tree_add_item(ndps_tree, hf_ndps_service_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Parameters");
            number_of_items=tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(atree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length!=0)
                {
                    proto_tree_add_item(atree, hf_ndps_item_bytes, tvb, foffset, length, ENC_NA);
                }
                foffset += length;
            }
            proto_item_set_end(aitem, tvb, foffset);
            break;
        case 0x00000005:    /
            proto_tree_add_item(ndps_tree, hf_ndps_list_services_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 0x00000006:    /
        case 0x00000007:    /
        case 0x00000008:    /
        default:
            break;
        }
        break;
    case 0x060978:  /
        switch(ndps_func)
        {
        case 0x00000001:    /
            foffset = credentials(tvb, pinfo, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_retrieve_restrictions, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            number_of_items=tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_bind_security_option_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Security %d", ii+1);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(atree, hf_bind_security, tvb, foffset, length, ENC_BIG_ENDIAN);
                }
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 0x00000002:    /
            break;
        case 0x00000003:    /
            foffset = server_entry(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000004:    /
        case 0x00000006:    /
        case 0x0000000b:    /
        case 0x0000000c:    /
            /
            break;
        case 0x00000005:    /
            foffset = ndps_string(tvb, hf_ndps_registry_name, ndps_tree, foffset, NULL);
            foffset = print_address(tvb, ndps_tree, foffset);
            break;
        case 0x00000007:    /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Add");
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(atree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Entry %u", ii+1);
                foffset = server_entry(tvb, pinfo, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Remove");
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(atree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Entry %u", ii+1);
                foffset = server_entry(tvb, pinfo, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            break;
        case 0x00000008:    /
        case 0x00000009:    /
        case 0x0000000a:    /
            local_servers_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_list_local_servers_type, tvb, foffset, 4, local_servers_type);
            foffset += 4;
            if (local_servers_type==0)
            {
                /
                integer_type_flag = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_integer_type_flag, tvb, foffset, 4, integer_type_flag);
                foffset += 4;
                if (integer_type_flag!=0)
                {
                    proto_tree_add_item(ndps_tree, hf_ndps_integer_type_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                }
                /
            }
            else
            {
                length = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_context_len, tvb, foffset, 4, length);
                foffset += 4;
                if (length!=0)
                {
                    proto_tree_add_item(ndps_tree, hf_ndps_context, tvb, foffset, length, ENC_NA);
                }
                foffset += length;
                foffset += (length%2);
                proto_tree_add_item(ndps_tree, hf_ndps_abort_flag, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
            }
            break;
        default:
            break;
        }
        break;
    case 0x060979:  /
        switch(ndps_func)
        {
        case 0x00000001:    /
            foffset = credentials(tvb, pinfo, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_retrieve_restrictions, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            number_of_items=tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Security %d", ii+1);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(atree, hf_bind_security, tvb, foffset, length, ENC_BIG_ENDIAN);
                }
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 0x00000002:    /
        case 0x0000000a:    /
        case 0x00000010:    /
        case 0x00000011:    /
            /
            break;
        case 0x00000003:    /
            foffset = ndps_string(tvb, hf_ndps_supplier_name, ndps_tree, foffset, NULL);
            /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Supplier Alias %u", ii+1);
                foffset = qualifiedname(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
            }
            /
            break;
        case 0x00000004:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 0x00000005:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Supplier Alias");
            foffset = qualifiedname(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_persistence, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Consumer Name");
            foffset = qualifiedname(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length==4)
            {
                proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, length, ENC_BIG_ENDIAN);
            }
            foffset += length;
            proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Method ID");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_delivery_add_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Delivery Addresses");
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, aitem, &ei_ndps_truncated);
                    break;
                }
                btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Address %d", ii+1);
                foffset = address_item(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            foffset = event_object_set(tvb, pinfo, ndps_tree, foffset);
            /
            break;
        case 0x00000006:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 0x00000007:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item_ret_uint(ndps_tree, hf_ndps_supplier_flag, tvb, foffset, 4, ENC_BIG_ENDIAN, &supplier_flag);
            foffset += 4;
            if (supplier_flag)
            {
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Supplier ID");
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(atree, hf_ndps_attribute_value, tvb, foffset, length, ENC_BIG_ENDIAN);
                }
                tvb_ensure_bytes_exist(tvb, foffset, length);
                foffset += length;
                proto_item_set_end(aitem, tvb, foffset);
            }
            proto_tree_add_item_ret_uint(ndps_tree, hf_ndps_language_flag, tvb, foffset, 4, ENC_BIG_ENDIAN, &language_flag);
            foffset += 4;
            if (language_flag)
            {
                proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
            }
            proto_tree_add_item_ret_uint(ndps_tree, hf_ndps_method_flag, tvb, foffset, 4, ENC_BIG_ENDIAN, &method_flag);
            foffset += 4;
            if (method_flag)
            {
                /
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Method ID");
                foffset = name_or_id(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
                /
            }
            proto_tree_add_item_ret_uint(ndps_tree, hf_ndps_delivery_address_flag, tvb, foffset, 4, ENC_BIG_ENDIAN, &delivery_address_flag);
            foffset += 4;
            if (delivery_address_flag)
            {
                foffset = print_address(tvb, ndps_tree, foffset);
            }
            foffset = event_object_set(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000008:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            profiles_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_list_profiles_type, tvb, foffset, 4, profiles_type);
            foffset += 4;
            if (profiles_type==0)   /
            {
                atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Supplier Alias");
                foffset = qualifiedname(tvb, atree, foffset);
                profiles_choice_type = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_list_profiles_choice_type, tvb, foffset, 4, profiles_choice_type);
                foffset += 4;
                proto_item_set_end(aitem, tvb, foffset);
                if (profiles_choice_type==0)   /
                {
                    foffset = cardinal_seq(tvb, pinfo, ndps_tree, foffset);
                }
                else
                {
                    atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Consumer");
                    foffset = qualifiedname(tvb, atree, foffset);
                    proto_item_set_end(aitem, tvb, foffset);
                    /
                    atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Method ID");
                    foffset = name_or_id(tvb, atree, foffset);
                    /
                    proto_tree_add_item(atree, hf_ndps_language_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                    proto_item_set_end(aitem, tvb, foffset);
                }
                proto_tree_add_item(ndps_tree, hf_ndps_list_profiles_result_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                /
                integer_type_flag = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_integer_type_flag, tvb, foffset, 4, integer_type_flag);
                foffset += 4;
                if (integer_type_flag!=0)
                {
                    proto_tree_add_item(ndps_tree, hf_ndps_integer_type_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                }
                /
            }
            else                                    /
            {
                length = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_context_len, tvb, foffset, 4, length);
                foffset += 4;
                if (length!=0)
                {
                    proto_tree_add_item(ndps_tree, hf_ndps_context, tvb, foffset, length, ENC_NA);
                }
                foffset += length;
                foffset += (length%2);
                proto_tree_add_item(ndps_tree, hf_ndps_abort_flag, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
            }
            break;
        case 0x00000009:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Event Items");
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, aitem, &ei_ndps_truncated);
                    break;
                }
                btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Item %d", ii+1);
                /
                proto_tree_add_item(btree, hf_ndps_event_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                ctree = proto_tree_add_subtree(btree, tvb, foffset, -1, ett_ndps, &citem, "Containing Class");
                foffset = objectidentifier(tvb, ctree, foffset);
                proto_item_set_end(citem, tvb, foffset);
                ctree = proto_tree_add_subtree(btree, tvb, foffset, -1, ett_ndps, &citem, "Containing Object");
                foffset = objectidentification(tvb, ctree, foffset);
                proto_item_set_end(citem, tvb, foffset);
                ctree = proto_tree_add_subtree(btree, tvb, foffset, -1, ett_ndps, &citem, "Filter Class");
                foffset = objectidentifier(tvb, ctree, foffset);
                ctree = proto_tree_add_subtree(btree, tvb, foffset, -1, ett_ndps, &citem, "Object Class");
                foffset = objectidentifier(tvb, ctree, foffset);
                proto_item_set_end(citem, tvb, foffset);
                ctree = proto_tree_add_subtree(btree, tvb, foffset, -1, ett_ndps, &citem, "Object ID");
                foffset = objectidentification(tvb, ctree, foffset);
                proto_item_set_end(citem, tvb, foffset);
                ctree = proto_tree_add_subtree(btree, tvb, foffset, -1, ett_ndps, &citem, "Event Object ID");
                foffset = objectidentifier(tvb, ctree, foffset);
                proto_item_set_end(citem, tvb, foffset);
                /
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(btree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
                foffset += 4;
                ctree = proto_tree_add_subtree(btree, tvb, foffset, -1, ett_ndps, &citem, "Attribute Modifications");
                for (jj = 0; jj < number_of_items; jj++ )
                {
                    if (jj >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, citem, &ei_ndps_truncated);
                        break;
                    }
                    dtree = proto_tree_add_subtree_format(ctree, tvb, foffset, -1, ett_ndps, &ditem, "Modification %d", jj+1);
                    foffset = attribute_value(tvb, pinfo, dtree, foffset);  /
                    proto_item_set_end(ditem, tvb, foffset);
                }
                proto_item_set_end(citem, tvb, foffset);
                /
                foffset = ndps_string(tvb, hf_ndps_message, btree, foffset, NULL);
                proto_tree_add_item(btree, hf_time, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_item_set_end(bitem, tvb, foffset);
                /
            }
            proto_item_set_end(aitem, tvb, foffset);
            /
            break;
        case 0x0000000b:    /
            /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_destinations, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Destination %d", ii+1);
                /
                /
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Method ID");
                foffset = name_or_id(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                /
                /
                proto_tree_add_item(atree, hf_address_len, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                foffset = print_address(tvb, atree, foffset);
                /
                proto_item_set_end(aitem, tvb, foffset);
                /
            }
            /
            foffset = ndps_string(tvb, hf_ndps_supplier_name, ndps_tree, foffset, NULL);
            proto_tree_add_item(ndps_tree, hf_ndps_event_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Containing Class");
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Containing Object");
            foffset = objectidentification(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Filter Class");
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Object Class");
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Object ID");
            foffset = objectidentification(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Event Object ID");
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Attributes");
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, aitem, &ei_ndps_truncated);
                    break;
                }
                btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Attribute %d", ii+1);
                foffset = attribute_value(tvb, pinfo, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = ndps_string(tvb, hf_ndps_message, ndps_tree, foffset, NULL);
            proto_tree_add_item(ndps_tree, hf_time, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Account");
            foffset = qualifiedname(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            break;
        case 0x0000000c:    /
            foffset = ndps_string(tvb, hf_ndps_file_name, ndps_tree, foffset, NULL);
            break;
        case 0x0000000d:    /
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Method ID");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            break;
        case 0x0000000e:    /
            cred_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_item(ndps_tree, hf_delivery_method_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            switch (cred_type)
            {
            case 0:        /
                /
                integer_type_flag = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_integer_type_flag, tvb, foffset, 4, integer_type_flag);
                foffset += 4;
                if (integer_type_flag!=0)
                {
                    proto_tree_add_item(ndps_tree, hf_ndps_integer_type_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                }
                /
                proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                break;
            case 1:       /
                length = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_context_len, tvb, foffset, 4, length);
                foffset += 4;
                if (length!=0)
                {
                    proto_tree_add_item(ndps_tree, hf_ndps_context, tvb, foffset, length, ENC_NA);
                }
                foffset += length;
                foffset += (length%2);
                proto_tree_add_item(ndps_tree, hf_ndps_abort_flag, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                break;
            default:
                break;
            }
            break;
        case 0x0000000f:    /
            /
            atree = proto_tree_add_subtree(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Method ID");
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        default:
            break;
        }
        break;
    case 0x06097a:  /
        switch(ndps_func)
        {
        case 0x00000001:    /
            foffset = credentials(tvb, pinfo, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_retrieve_restrictions, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            number_of_items=tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_bind_security_option_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Security %d", ii+1);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(atree, hf_bind_security, tvb, foffset, length, ENC_BIG_ENDIAN);
                }
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 0x00000002:    /
        case 0x00000008:    /
        case 0x00000009:    /
            /
            break;
        case 0x00000003:    /
            proto_tree_add_item(ndps_tree, hf_packet_count, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_last_packet_flag, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_file_timestamp, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = res_add_input_data(tvb, ndps_tree, foffset);
            number_of_items=tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Item %d", ii+1);
                length=tvb_get_ntohl(tvb, foffset);
                length_remaining = tvb_reported_length_remaining(tvb, foffset);
                if(length_remaining == -1 || (guint32) length_remaining < length)
                {
                    return foffset;
                }
                proto_tree_add_item(atree, hf_ndps_item_ptr, tvb, foffset, length, ENC_NA);
                foffset += length;
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 0x00000004:    /
            foffset = res_add_input_data(tvb, ndps_tree, foffset);
            break;
        case 0x00000005:    /
            proto_tree_add_item(ndps_tree, hf_ndps_max_items, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_status_flags, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_resource_list_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
            resource_type = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            switch (resource_type)
            {
            case 0:     /
                proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                break;
            case 1:     /
            case 2:     /
                foffset = ndps_string(tvb, hf_ndps_vendor_dir, ndps_tree, foffset, NULL);
                break;
            case 3:     /
                proto_tree_add_item(ndps_tree, hf_banner_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                break;
            case 4:     /
                proto_tree_add_item(ndps_tree, hf_font_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                break;
            case 5:     /
            case 12:    /
            case 9:     /
                proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                foffset = ndps_string(tvb, hf_ndps_printer_type, ndps_tree, foffset, NULL);
                foffset = ndps_string(tvb, hf_ndps_printer_manuf, ndps_tree, foffset, NULL);
                foffset = ndps_string(tvb, hf_ndps_inf_file_name, ndps_tree, foffset, NULL);
                field_len = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                proto_tree_add_item(ndps_tree, hf_printer_id, tvb, foffset, field_len, ENC_NA);
                break;
            case 6:     /
            case 10:    /
                foffset = ndps_string(tvb, hf_ndps_vendor_dir, ndps_tree, foffset, NULL);
                foffset += 4;
                foffset = ndps_string(tvb, hf_ndps_printer_type, ndps_tree, foffset, NULL);
                foffset = ndps_string(tvb, hf_ndps_printer_manuf, ndps_tree, foffset, NULL);
                foffset = ndps_string(tvb, hf_ndps_inf_file_name, ndps_tree, foffset, NULL);
                field_len = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                proto_tree_add_item(ndps_tree, hf_printer_id, tvb, foffset, field_len, ENC_NA);
                break;
            case 7:     /
                proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_tree_add_item(ndps_tree, hf_font_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                foffset = ndps_string(tvb, hf_ndps_font_name, ndps_tree, foffset, NULL);
                break;
            case 8:     /
            case 11:    /
            case 13:    /
                foffset = ndps_string(tvb, hf_ndps_printer_manuf, ndps_tree, foffset, NULL);
                foffset = ndps_string(tvb, hf_ndps_printer_type, ndps_tree, foffset, NULL);
                foffset = ndps_string(tvb, hf_ndps_inf_file_name, ndps_tree, foffset, NULL);
                break;
            case 14:    /
                break;
            default:
                break;
            }
            break;
        case 0x00000006:    /
            proto_tree_add_item(ndps_tree, hf_get_status_flag, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_res_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
            resource_type = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            switch (resource_type)
            {
            case 0:     /
                proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                foffset = ndps_string(tvb, hf_ndps_prn_dir_name, ndps_tree, foffset, NULL);
                foffset = ndps_string(tvb, hf_ndps_prn_file_name, ndps_tree, foffset, NULL);
                break;
            case 1:     /
                foffset = ndps_string(tvb, hf_ndps_vendor_dir, ndps_tree, foffset, NULL);
                foffset = ndps_string(tvb, hf_ndps_prn_file_name, ndps_tree, foffset, NULL);
                break;
            case 2:     /
                foffset = ndps_string(tvb, hf_ndps_banner_name, ndps_tree, foffset, NULL);
                break;
            case 3:     /
                proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_tree_add_item(ndps_tree, hf_font_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                foffset = ndps_string(tvb, hf_ndps_prn_file_name, ndps_tree, foffset, NULL);
                break;
            case 4:     /
            case 5:     /
                proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                foffset = ndps_string(tvb, hf_ndps_prn_dir_name, ndps_tree, foffset, NULL);
                proto_tree_add_item(ndps_tree, hf_archive_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                break;
            default:
                break;
            }
            break;
        case 0x00000007:    /
            proto_tree_add_item(ndps_tree, hf_ndps_status_flags, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = res_add_input_data(tvb, ndps_tree, foffset);
            break;
        case 0x0000000a:    /
            proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        default:
            break;
        }
        break;
    case 0x06097b:  /
        switch(ndps_func)
        {
        case 0x00000001:    /
            foffset = credentials(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000002:    /
            /
            break;
        case 0x00000003:    /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Item %d", ii+1);
                proto_tree_add_item(atree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Supplier ID");
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(btree, hf_ndps_attribute_value, tvb, foffset, length, ENC_BIG_ENDIAN);
                }
                foffset += length;
                proto_tree_add_item(btree, hf_ndps_event_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_item_set_end(bitem, tvb, foffset);
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Containing Class");
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Containing Object");
                foffset = objectidentification(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Filter Class");
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Object Class");
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Object ID");
                foffset = objectidentification(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Event Object ID");
                foffset = objectidentifier(tvb, btree, foffset);
                foffset = attribute_value(tvb, pinfo, atree, foffset);
                foffset = ndps_string(tvb, hf_ndps_message, atree, foffset, NULL);
                proto_tree_add_item(atree, hf_time, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_item_set_end(bitem, tvb, foffset);
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Account");
                foffset = qualifiedname(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 0x00000004:    /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Object %d", ii+1);
                proto_tree_add_item(atree, hf_ndps_session, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Supplier ID");
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(btree, hf_ndps_attribute_value, tvb, foffset, length, ENC_BIG_ENDIAN);
                }
                foffset += length;
                proto_tree_add_item(atree, hf_ndps_event_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_item_set_end(bitem, tvb, foffset);
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Containing Class");
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Containing Object");
                foffset = objectidentification(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Filter Class");
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Object Class");
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Object ID");
                foffset = objectidentification(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Event Object ID");
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                /
                number_of_items2 = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items2);
                foffset += 4;
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Attribute");
                for (jj = 0; jj < number_of_items2; jj++ )
                {
                    if (jj >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, bitem, &ei_ndps_truncated);
                        break;
                    }
                    foffset = attribute_value(tvb, pinfo, btree, foffset);
                }
                proto_item_set_end(bitem, tvb, foffset);
                /
                foffset = ndps_string(tvb, hf_ndps_message, atree, foffset, NULL);
                proto_tree_add_item(atree, hf_time, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                btree = proto_tree_add_subtree(atree, tvb, foffset, -1, ett_ndps, &bitem, "Account");
                foffset = qualifiedname(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return foffset;
}
