static void
dissect_ndps_request(tvbuff_t *tvb, packet_info *pinfo, proto_tree *ndps_tree, guint32 ndps_prog, guint32 ndps_func, int foffset)
{
    ndps_req_hash_value	*request_value = NULL;
    conversation_t      *conversation;
    guint32             i;
    guint32             j;
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
    gint	        length_remaining;
    proto_tree          *atree;
    proto_item          *aitem;
    proto_tree          *btree;
    proto_item          *bitem;
    proto_tree          *ctree;
    proto_item          *citem;
    proto_tree          *dtree;
    proto_item          *ditem;

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
        }

        request_value = ndps_hash_insert(conversation, (guint32) pinfo->srcport);
        request_value->ndps_prog = ndps_prog;
        request_value->ndps_func = ndps_func;
        request_value->ndps_frame_num = pinfo->fd->num;
    }
    switch(ndps_prog)
    {
    case 0x060976:  /
        switch(ndps_func)
        {
        case 0x00000001:    /
            foffset = credentials(tvb, ndps_tree, foffset);
            break;
        case 0x00000002:    /
            foffset = credentials(tvb, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_retrieve_restrictions, tvb, foffset, 4, FALSE);
            foffset += 4;
            number_of_items=tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_bind_security_option_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Security %u", i);
                atree = proto_item_add_subtree(aitem, ett_ndps);
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
            4, FALSE);
            break;
        case 0x00000004:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            print_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_print_arg, tvb, foffset, 4, print_type);
            foffset += 4;
            switch (print_type) 
            {
            case 0:     /
                foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL, 0);
                proto_tree_add_item(ndps_tree, hf_sub_complete, tvb, foffset, 4, FALSE);
                foffset += 4;
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Transfer Method");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Attribute %u", i);
                    btree = proto_item_add_subtree(bitem, ett_ndps);
                    foffset = objectidentifier(tvb, btree, foffset);
                    number_of_items2 = tvb_get_ntohl(tvb, foffset);
                    proto_tree_add_uint(btree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items2);
                    foffset += 4;
                    for (j = 1 ; j <= number_of_items2; j++ )
                    {
	                if (j > NDPS_MAX_ITEMS) {
	                    proto_tree_add_text(btree, tvb, foffset, -1, "[Truncated]");
	                    break;
	                }
                        citem = proto_tree_add_text(btree, tvb, foffset, -1, "Value %u", j);
                        ctree = proto_item_add_subtree(citem, ett_ndps);
                        foffset = attribute_value(tvb, ctree, foffset);
                        proto_item_set_end(citem, tvb, foffset);
                    }
                    proto_tree_add_item(btree, hf_ndps_qualifier, tvb, foffset, 4, FALSE);
                    foffset += 4;
                    proto_item_set_end(bitem, tvb, foffset);
                }
                proto_item_set_end(aitem, tvb, foffset);
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Document Content");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Type %u", i);
                    btree = proto_item_add_subtree(bitem, ett_ndps);
                    foffset = objectidentifier(tvb, btree, foffset);
                    proto_item_set_end(bitem, tvb, foffset);
                }
                foffset += align_4(tvb, foffset);
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items);
                foffset += 4;
                doc_content = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_doc_content, tvb, foffset, 4, doc_content);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Value %u", i);
                    btree = proto_item_add_subtree(bitem, ett_ndps);
                    if (doc_content==0)
                    {
                        length = tvb_get_ntohl(tvb, foffset);
                        proto_tree_add_uint(btree, hf_ndps_included_doc_len, tvb, foffset, 4, length);
                        foffset += 4;
                        length_remaining = tvb_length_remaining(tvb, foffset);
                        if (length_remaining == -1 || length > (guint32) length_remaining) /
                        {
                            proto_tree_add_item(btree, hf_ndps_data, tvb, foffset, -1, FALSE);
                            return;
                        }
                        if (length==4)
                        {
                            proto_tree_add_item(btree, hf_ndps_included_doc, tvb, foffset, length, FALSE);
                        }
                        foffset += length;
                        foffset += (length%2);
                        if ((int) foffset <= 0)
                            THROW(ReportedBoundsError);
                    }
                    else
                    {
                        foffset = ndps_string(tvb, hf_ndps_ref_name, btree, foffset, NULL, 0);
                        foffset = name_or_id(tvb, btree, foffset);
                    }
                    proto_item_set_end(bitem, tvb, foffset);
                }
                proto_item_set_end(aitem, tvb, foffset);
                foffset += 4;
                if (align_4(tvb, foffset)>0) {
                    foffset += align_4(tvb, foffset);
                }
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Document Type");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Attribute %u", i);
                    btree = proto_item_add_subtree(bitem, ett_ndps);
                    foffset = objectidentifier(tvb, btree, foffset);
                    number_of_items2 = tvb_get_ntohl(tvb, foffset);
                    proto_tree_add_uint(btree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items2);
                    foffset += 4;
                    for (j = 1 ; j <= number_of_items2; j++ )
                    {
	                if (j > NDPS_MAX_ITEMS) {
	                    proto_tree_add_text(btree, tvb, foffset, -1, "[Truncated]");
	                    break;
	                }
                        citem = proto_tree_add_text(btree, tvb, foffset, -1, "Value %u", j);
                        ctree = proto_item_add_subtree(citem, ett_ndps);
                        foffset = attribute_value(tvb, ctree, foffset);
                        proto_item_set_end(citem, tvb, foffset);
                    }
                    proto_tree_add_item(btree, hf_ndps_qualifier, tvb, foffset, 4, FALSE);
                    foffset += 4;
                    proto_item_set_end(bitem, tvb, foffset);
                }
                proto_item_set_end(aitem, tvb, foffset);
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Document Attributes");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Attribute %u", i);
                    btree = proto_item_add_subtree(bitem, ett_ndps);
                    foffset = attribute_value(tvb, btree, foffset);  /
                    proto_item_set_end(bitem, tvb, foffset);
                }
                break;
            case 1:     /
                foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL, 0);
                proto_tree_add_item(ndps_tree, hf_local_id, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_tree_add_item(ndps_tree, hf_sub_complete, tvb, foffset, 4, FALSE);
                foffset += 4;
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Transfer Method");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_num_transfer_methods, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Method %u", i);
                    btree = proto_item_add_subtree(bitem, ett_ndps);
                    foffset = objectidentifier(tvb, btree, foffset); /
                    proto_item_set_end(bitem, tvb, foffset);
                }
                proto_tree_add_item(ndps_tree, hf_doc_content, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_item_set_end(aitem, tvb, foffset);
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Document Type");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_num_doc_types, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Type %u", i);
                    btree = proto_item_add_subtree(bitem, ett_ndps);
                    foffset = objectidentifier(tvb, btree, foffset); /
                    proto_item_set_end(bitem, tvb, foffset);
                }
                foffset += align_4(tvb, foffset);
                proto_item_set_end(aitem, tvb, foffset);
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Document Attributes");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Attribute %u", i);
                    btree = proto_item_add_subtree(bitem, ett_ndps);
                    foffset = attribute_value(tvb, btree, foffset);  /
                    proto_item_set_end(bitem, tvb, foffset);
                }
                proto_item_set_end(aitem, tvb, foffset);
                break;
            case 2:     /
                foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL, 0);
                proto_tree_add_item(ndps_tree, hf_local_id, tvb, foffset, 4, FALSE);
                foffset += 4;
                break;
            default:
                break;
            }
            break;
        case 0x00000005:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL, 0);
            proto_tree_add_item(ndps_tree, hf_local_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_document_number, tvb, foffset, 4, FALSE);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Job Modifications");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(atree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Modification %u", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = attribute_value(tvb, btree, foffset);  /
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Document Modifications");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(atree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Modification %u", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = attribute_value(tvb, btree, foffset);  /
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            break;
        case 0x00000006:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL, 0);
            proto_tree_add_item(ndps_tree, hf_local_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_document_number, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Cancel Message");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Retention Period");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            proto_tree_add_item(atree, hf_ndps_status_flags, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(atree, hf_ndps_attribute_value, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_item_set_end(aitem, tvb, foffset);
            break;
        case 0x00000007:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
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
                    tvb_ensure_bytes_exist(tvb, foffset, length);
                    proto_tree_add_item(ndps_tree, hf_ndps_context, tvb, foffset, length, FALSE);
                }
                foffset += length;
                foffset += (length%2);
                proto_tree_add_item(ndps_tree, hf_ndps_abort_flag, tvb, foffset, 4, FALSE);
                foffset += 4;
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Attribute %u", i);
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    foffset = attribute_value(tvb, atree, foffset);
                    proto_item_set_end(aitem, tvb, foffset);
                }
            }
            else                                  /
            {
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Object Class");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                foffset = objectidentifier(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
                foffset += 4;
                foffset += align_4(tvb, foffset);
                scope = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_scope, tvb, foffset, 4, scope);
                foffset += 4;
                if (scope!=0)    /
                {
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Selector Option");
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    number_of_items = tvb_get_ntohl(tvb, foffset); /
                    proto_tree_add_uint(atree, hf_ndps_num_options, tvb, foffset, 4, number_of_items);
                    foffset += 4;
                    for (i = 1 ; i <= number_of_items; i++ )
                    {
	                if (i > NDPS_MAX_ITEMS) {
	                    proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	                    break;
	                }
                        bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Option %u", i);
                        btree = proto_item_add_subtree(bitem, ett_ndps);
                        foffset = objectidentification(tvb, btree, foffset);
                        proto_item_set_end(bitem, tvb, foffset);
                    }
                    proto_item_set_end(aitem, tvb, foffset);
                    foffset += align_4(tvb, foffset);
                    filter_type = tvb_get_ntohl(tvb, foffset);
                    proto_tree_add_uint(ndps_tree, hf_ndps_filter, tvb, foffset, 4, filter_type);
                    foffset += 4;
                    /
                    proto_tree_add_item(ndps_tree, hf_ndps_time_limit, tvb, foffset, 4, FALSE);
                    foffset += 4;
                    proto_tree_add_item(ndps_tree, hf_ndps_count_limit, tvb, foffset, 4, FALSE);
                    foffset += 4; /
                }
                foffset += 4;   /
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Requested Attributes");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                number_of_items = tvb_get_ntohl(tvb, foffset); /
                proto_tree_add_uint(atree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Attribute %u", i);
                    btree = proto_item_add_subtree(bitem, ett_ndps);
                    foffset = objectidentifier(tvb, btree, foffset);
                    proto_item_set_end(bitem, tvb, foffset);
                }
                proto_item_set_end(aitem, tvb, foffset); /
                if (number_of_items == 0) 
                {
                    break;
                }
                proto_tree_add_item(ndps_tree, hf_ndps_operator, tvb, foffset, 4, FALSE);
                foffset += 4;
                foffset = commonarguments(tvb, ndps_tree, foffset);
            }
            break;
        case 0x00000008:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Job ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = ndps_string(tvb, hf_ndps_pa_name, atree, foffset, NULL, 0);
            proto_tree_add_item(atree, hf_local_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_item_set_end(aitem, tvb, foffset);
            /
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Message Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x00000009:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            job_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_interrupt_job_type, tvb, foffset, 4, job_type);
            foffset += 4;
            if (job_type==0)
            {
                /
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Job ID");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                foffset = ndps_string(tvb, hf_ndps_pa_name, atree, foffset, NULL, 0);
                proto_tree_add_item(atree, hf_local_id, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_item_set_end(aitem, tvb, foffset);
                /
            }
            else
            {
                foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL, 0);
            }
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Interrupt Message Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Interrupting Job");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = ndps_string(tvb, hf_ndps_pa_name, atree, foffset, NULL, 0);
            proto_tree_add_item(atree, hf_local_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x0000000a:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            job_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_pause_job_type, tvb, foffset, 4, job_type);
            foffset += 4;
            if (job_type==0)
            {
                /
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Job ID");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                foffset = ndps_string(tvb, hf_ndps_pa_name, atree, foffset, NULL, 0);
                proto_tree_add_item(atree, hf_local_id, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_item_set_end(aitem, tvb, foffset);
                /
            }
            else
            {
                foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL, 0);
            }
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Pause Message Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x0000000b:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Job ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = ndps_string(tvb, hf_ndps_pa_name, atree, foffset, NULL, 0);
            proto_tree_add_item(atree, hf_local_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_item_set_end(aitem, tvb, foffset);
            /
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Resume Message Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x0000000c:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Clean Message Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x0000000d:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Object Class");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Object ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentification(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_force, tvb, foffset, 4, FALSE);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Reference Object Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentification(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Object Attribute");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(atree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Attribute %u", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = attribute_value(tvb, btree, foffset);  /
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x0000000e:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Object Class");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Object ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentification(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x0000000f:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Disable PA Message Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x00000010:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Enable PA Message Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x00000011:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            foffset = qualifiedname(tvb, ndps_tree, foffset);
            foffset = address_item(tvb, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_resubmit_op_type, tvb, foffset, 4, FALSE);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Resubmit Job");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items = tvb_get_ntohl(tvb, foffset); /
            proto_tree_add_uint(atree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                /
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Job ID");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = ndps_string(tvb, hf_ndps_pa_name, btree, foffset, NULL, 0);
                proto_tree_add_item(btree, hf_local_id, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_item_set_end(bitem, tvb, foffset);
                /
                proto_tree_add_item(atree, hf_ndps_document_number, tvb, foffset, 4, FALSE);
                foffset += 4;
                /
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Job Attributes");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                number_of_items2 = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(btree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items2);
                foffset += 4;
                for (j = 1 ; j <= number_of_items2; j++ )
                {
	            if (j > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(btree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    citem = proto_tree_add_text(btree, tvb, foffset, -1, "Attribute %u", j);
                    ctree = proto_item_add_subtree(citem, ett_ndps);
                    foffset = attribute_value(tvb, ctree, foffset);  /
                    proto_item_set_end(citem, tvb, foffset);
                }
                proto_item_set_end(bitem, tvb, foffset);
                /
                /
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Document Attributes");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                number_of_items2 = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(btree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items2);
                foffset += 4;
                for (j = 1 ; j <= number_of_items2; j++ )
                {
	            if (j > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(btree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    citem = proto_tree_add_text(btree, tvb, foffset, -1, "Attribute %u", j);
                    ctree = proto_item_add_subtree(citem, ett_ndps);
                    foffset = attribute_value(tvb, ctree, foffset);  /
                    proto_item_set_end(citem, tvb, foffset);
                }
                proto_item_set_end(bitem, tvb, foffset);
                /
            }
            proto_item_set_end(aitem, tvb, foffset);	/
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Resubmit Message Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x00000012:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Object Class");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Object ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentification(tvb, atree, foffset);
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Attribute Modifications");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(atree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Modification %d", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = attribute_value(tvb, btree, foffset);  /
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x00000013:    /
        case 0x0000001e:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_shutdown_type, tvb, foffset, 4, FALSE);
            foffset += 4;
            foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL, 0);
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Shutdown Message Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
        case 0x00000014:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL, 0);
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Startup Message Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x00000015:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Job Identification");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = ndps_string(tvb, hf_ndps_pa_name, atree, foffset, NULL, 0);
            proto_tree_add_item(atree, hf_local_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_item_set_end(aitem, tvb, foffset);
            /
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Reference Job ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = ndps_string(tvb, hf_ndps_pa_name, atree, foffset, NULL, 0);
            proto_tree_add_item(atree, hf_local_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x00000016:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Pause Message Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x00000017:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Resume Message Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x00000018:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_get_status_flag, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_data, tvb, foffset+4, tvb_get_ntohl(tvb, foffset), FALSE);
            break;
        case 0x00000019:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Operation ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x0000001a:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_persistence, tvb, foffset, 4, FALSE);
            foffset += 4;
            foffset = qualifiedname(tvb, ndps_tree, foffset);
            foffset = ndps_string(tvb, hf_ndps_supplier_name, ndps_tree, foffset, NULL, 0);
            proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Method ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            foffset += align_4(tvb, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Delivery Address");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(atree, hf_ndps_delivery_add_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Address %u", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = address_item(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            foffset = event_object_set(tvb, ndps_tree, foffset);
            foffset = qualifiedname(tvb, ndps_tree, foffset);
            break;
        case 0x0000001b:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            break;
        case 0x0000001c:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            supplier_flag = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_boolean(ndps_tree, hf_ndps_supplier_flag, tvb, foffset, 4, supplier_flag);
            foffset += 4;
            if (supplier_flag)
            {
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Supplier ID");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(atree, hf_ndps_attribute_value, tvb, foffset, length, FALSE);
                }
                tvb_ensure_bytes_exist(tvb, foffset, length);
                foffset += length;
                proto_item_set_end(aitem, tvb, foffset);
            }
            language_flag = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_boolean(ndps_tree, hf_ndps_language_flag, tvb, foffset, 4, language_flag);
            foffset += 4;
            if (language_flag)
            {
                proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, FALSE);
                foffset += 4;
            }
            method_flag = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_boolean(ndps_tree, hf_ndps_method_flag, tvb, foffset, 4, method_flag);
            foffset += 4;
            if (method_flag)
            {
                /
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Method ID");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                foffset = name_or_id(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
                /
            }
            delivery_address_flag = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_boolean(ndps_tree, hf_ndps_delivery_address_flag, tvb, foffset, 4, delivery_address_flag);
            foffset += 4;
            if (delivery_address_flag)
            {
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Delivery Address");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                foffset = print_address(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
            }
            foffset = event_object_set(tvb, ndps_tree, foffset);
            break;
        case 0x0000001d:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
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
                    foffset = cardinal_seq(tvb, ndps_tree, foffset);
                }
                else
                {
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Consumer");
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    foffset = qualifiedname(tvb, atree, foffset);
                    proto_item_set_end(aitem, tvb, foffset);
                    /
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Method ID");
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    foffset = name_or_id(tvb, atree, foffset);
                    /
                    proto_tree_add_item(atree, hf_ndps_language_id, tvb, foffset, 4, FALSE);
                    foffset += 4;
                    proto_item_set_end(aitem, tvb, foffset);
                }
                proto_tree_add_item(ndps_tree, hf_ndps_list_profiles_result_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                /
                integer_type_flag = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_integer_type_flag, tvb, foffset, 4, integer_type_flag);
                foffset += 4;
                if (integer_type_flag!=0) 
                {
                    proto_tree_add_item(ndps_tree, hf_ndps_integer_type_value, tvb, foffset, 4, FALSE);
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
                    tvb_ensure_bytes_exist(tvb, foffset, length);
                    proto_tree_add_item(ndps_tree, hf_ndps_context, tvb, foffset, length, FALSE);
                }
                foffset += length;
                foffset += (length%2);
                proto_tree_add_item(ndps_tree, hf_ndps_abort_flag, tvb, foffset, 4, FALSE);
                foffset += 4;
            }
            break;
        case 0x0000001f:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Cancel Shutdown Message Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x00000020:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_ds_info_type, tvb, foffset, 4, FALSE);
            foffset += 4;
            foffset = ndps_string(tvb, hf_printer_name, ndps_tree, foffset, NULL, 0);
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "DS Object Name");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = qualifiedname(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            break;
        case 0x00000021:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Clean Message Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = commonarguments(tvb, ndps_tree, foffset);
            break;
        case 0x00000022:    /
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length!=0)
            {
                tvb_ensure_bytes_exist(tvb, foffset, length);
                proto_tree_add_item(ndps_tree, hf_ndps_guid, tvb, foffset, length, FALSE);
            }
            foffset += length;
            break;
        case 0x00000023:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_persistence, tvb, foffset, 4, FALSE);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Consumer Name");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = qualifiedname(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            foffset = ndps_string(tvb, hf_ndps_supplier_name, ndps_tree, foffset, NULL, 0);
            proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Method ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            foffset += align_4(tvb, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Delivery Address");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(atree, hf_ndps_delivery_add_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Address %u", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = address_item(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            foffset = event_object_set(tvb, ndps_tree, foffset);
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Account");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = qualifiedname(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Notify Attributes");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(atree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Attribute %u", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            /
            proto_tree_add_item(ndps_tree, hf_notify_time_interval, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_notify_sequence_number, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_notify_lease_exp_time, tvb, foffset, 4, FALSE);
            foffset += 4;
            foffset = ndps_string(tvb, hf_notify_printer_uri, ndps_tree, foffset, NULL, 0);
            /
            break;
        case 0x00000024:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            profiles_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_list_profiles_type, tvb, foffset, 4, profiles_type);
            foffset += 4;
            if (profiles_type==0)   /
            {
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Supplier Alias");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                foffset = qualifiedname(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
                profiles_choice_type = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_list_profiles_choice_type, tvb, foffset, 4, profiles_choice_type);
                foffset += 4;
                if (profiles_choice_type==0)   /
                {
                    foffset = cardinal_seq(tvb, ndps_tree, foffset);
                }
                else
                {
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Consumer");
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    foffset = qualifiedname(tvb, atree, foffset);
                    proto_item_set_end(aitem, tvb, foffset);
                    /
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Method ID");
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    foffset = name_or_id(tvb, atree, foffset);
                    proto_item_set_end(aitem, tvb, foffset);
                    /
                    proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, FALSE);
                    foffset += 4;
                }
                proto_tree_add_item(ndps_tree, hf_ndps_list_profiles_result_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                /
                integer_type_flag = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_integer_type_flag, tvb, foffset, 4, integer_type_flag);
                foffset += 4;
                if (integer_type_flag!=0) 
                {
                    proto_tree_add_item(ndps_tree, hf_ndps_integer_type_value, tvb, foffset, 4, FALSE);
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
                    tvb_ensure_bytes_exist(tvb, foffset, length);
                    proto_tree_add_item(ndps_tree, hf_ndps_context, tvb, foffset, length, FALSE);
                }
                foffset += length;
                foffset += (length%2);
                proto_tree_add_item(ndps_tree, hf_ndps_abort_flag, tvb, foffset, 4, FALSE);
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
            foffset = credentials(tvb, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_retrieve_restrictions, tvb, foffset, 4, FALSE);
            foffset += 4;
            number_of_items=tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_bind_security_option_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Security %u", i);
                atree = proto_item_add_subtree(aitem, ett_ndps);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(atree, hf_bind_security, tvb, foffset, length, FALSE);
                }
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 0x00000002:    /
            break;
        case 0x00000003:    /
            proto_tree_add_item(ndps_tree, hf_ndps_list_services_type, tvb, foffset, 4, FALSE);
            foffset += 4;
            break;
        case 0x00000004:    /
            proto_tree_add_item(ndps_tree, hf_ndps_service_type, tvb, foffset, 4, FALSE);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Parameters");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items=tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(atree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length!=0)
                {
                    tvb_ensure_bytes_exist(tvb, foffset, length);
                    proto_tree_add_item(atree, hf_ndps_item_bytes, tvb, foffset, length, FALSE);
                }
                foffset += length;
            }
            proto_item_set_end(aitem, tvb, foffset);
            break;
        case 0x00000005:    /
            proto_tree_add_item(ndps_tree, hf_ndps_list_services_type, tvb, foffset, 4, FALSE);
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
            foffset = credentials(tvb, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_retrieve_restrictions, tvb, foffset, 4, FALSE);
            foffset += 4;
            number_of_items=tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_bind_security_option_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Security %d", i);
                atree = proto_item_add_subtree(aitem, ett_ndps);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(atree, hf_bind_security, tvb, foffset, length, FALSE);
                }
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 0x00000002:    /
            break;
        case 0x00000003:    /
            foffset = server_entry(tvb, ndps_tree, foffset);
            break;
        case 0x00000004:    /
        case 0x00000006:    /
        case 0x0000000b:    /
        case 0x0000000c:    /
            /
            break;
        case 0x00000005:    /
            foffset = ndps_string(tvb, hf_ndps_registry_name, ndps_tree, foffset, NULL, 0);
            foffset = print_address(tvb, ndps_tree, foffset);
            break;
        case 0x00000007:    /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Add");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(atree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Entry %u", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = server_entry(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Remove");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(atree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Entry %u", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = server_entry(tvb, btree, foffset);
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
                    proto_tree_add_item(ndps_tree, hf_ndps_integer_type_value, tvb, foffset, 4, FALSE);
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
                    tvb_ensure_bytes_exist(tvb, foffset, length);
                    proto_tree_add_item(ndps_tree, hf_ndps_context, tvb, foffset, length, FALSE);
                }
                foffset += length;
                foffset += (length%2);
                proto_tree_add_item(ndps_tree, hf_ndps_abort_flag, tvb, foffset, 4, FALSE);
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
            foffset = credentials(tvb, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_retrieve_restrictions, tvb, foffset, 4, FALSE);
            foffset += 4;
            number_of_items=tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Security %d", i);
                atree = proto_item_add_subtree(aitem, ett_ndps);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(atree, hf_bind_security, tvb, foffset, length, FALSE);
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
            foffset = ndps_string(tvb, hf_ndps_supplier_name, ndps_tree, foffset, NULL, 0);
            /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Supplier Alias %u", i);
                atree = proto_item_add_subtree(aitem, ett_ndps);
                foffset = qualifiedname(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
            }
            /
            break;
        case 0x00000004:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            break;
        case 0x00000005:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Supplier Alias");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = qualifiedname(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_persistence, tvb, foffset, 4, FALSE);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Consumer Name");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = qualifiedname(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length==4)
            {
                proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, length, FALSE);
            }
            foffset += length;
            proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Method ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_delivery_add_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Delivery Addresses");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Address %d", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = address_item(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            foffset = event_object_set(tvb, ndps_tree, foffset);
            /
            break;
        case 0x00000006:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            break;
        case 0x00000007:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            supplier_flag = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_boolean(ndps_tree, hf_ndps_supplier_flag, tvb, foffset, 4, supplier_flag);
            foffset += 4;
            if (supplier_flag)
            {
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Supplier ID");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(atree, hf_ndps_attribute_value, tvb, foffset, length, FALSE);
                }
                tvb_ensure_bytes_exist(tvb, foffset, length);
                foffset += length;
                proto_item_set_end(aitem, tvb, foffset);
            }
            language_flag = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_boolean(ndps_tree, hf_ndps_language_flag, tvb, foffset, 4, language_flag);
            foffset += 4;
            if (language_flag)
            {
                proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, FALSE);
                foffset += 4;
            }
            method_flag = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_boolean(ndps_tree, hf_ndps_method_flag, tvb, foffset, 4, method_flag);
            foffset += 4;
            if (method_flag)
            {
                /
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Method ID");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                foffset = name_or_id(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
                /
            }
            delivery_address_flag = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_boolean(ndps_tree, hf_ndps_delivery_address_flag, tvb, foffset, 4, delivery_address_flag);
            foffset += 4;
            if (delivery_address_flag)
            {
                foffset = print_address(tvb, ndps_tree, foffset);
            }
            foffset = event_object_set(tvb, ndps_tree, foffset);
            break;
        case 0x00000008:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            profiles_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_list_profiles_type, tvb, foffset, 4, profiles_type);
            foffset += 4;
            if (profiles_type==0)   /
            {
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Supplier Alias");
                atree = proto_item_add_subtree(aitem, ett_ndps);
                foffset = qualifiedname(tvb, atree, foffset);
                profiles_choice_type = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_list_profiles_choice_type, tvb, foffset, 4, profiles_choice_type);
                foffset += 4;
                proto_item_set_end(aitem, tvb, foffset);
                if (profiles_choice_type==0)   /
                {
                    foffset = cardinal_seq(tvb, ndps_tree, foffset);
                }
                else
                {
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Consumer");
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    foffset = qualifiedname(tvb, atree, foffset);
                    proto_item_set_end(aitem, tvb, foffset);
                    /
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Method ID");
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    foffset = name_or_id(tvb, atree, foffset);
                    /
                    proto_tree_add_item(atree, hf_ndps_language_id, tvb, foffset, 4, FALSE);
                    foffset += 4;
                    proto_item_set_end(aitem, tvb, foffset);
                }
                proto_tree_add_item(ndps_tree, hf_ndps_list_profiles_result_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                /
                integer_type_flag = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_integer_type_flag, tvb, foffset, 4, integer_type_flag);
                foffset += 4;
                if (integer_type_flag!=0) 
                {
                    proto_tree_add_item(ndps_tree, hf_ndps_integer_type_value, tvb, foffset, 4, FALSE);
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
                    tvb_ensure_bytes_exist(tvb, foffset, length);
                    proto_tree_add_item(ndps_tree, hf_ndps_context, tvb, foffset, length, FALSE);
                }
                foffset += length;
                foffset += (length%2);
                proto_tree_add_item(ndps_tree, hf_ndps_abort_flag, tvb, foffset, 4, FALSE);
                foffset += 4;
            }
            break;
        case 0x00000009:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Event Items");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Item %d", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                /
                proto_tree_add_item(btree, hf_ndps_event_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                citem = proto_tree_add_text(btree, tvb, foffset, -1, "Containing Class");
                ctree = proto_item_add_subtree(citem, ett_ndps);
                foffset = objectidentifier(tvb, ctree, foffset);
                proto_item_set_end(citem, tvb, foffset);
                citem = proto_tree_add_text(btree, tvb, foffset, -1, "Containing Object");
                ctree = proto_item_add_subtree(citem, ett_ndps);
                foffset = objectidentification(tvb, ctree, foffset);
                proto_item_set_end(citem, tvb, foffset);
                citem = proto_tree_add_text(btree, tvb, foffset, -1, "Filter Class");
                ctree = proto_item_add_subtree(citem, ett_ndps);
                foffset = objectidentifier(tvb, ctree, foffset);
                citem = proto_tree_add_text(btree, tvb, foffset, -1, "Object Class");
                ctree = proto_item_add_subtree(citem, ett_ndps);
                foffset = objectidentifier(tvb, ctree, foffset);
                proto_item_set_end(citem, tvb, foffset);
                citem = proto_tree_add_text(btree, tvb, foffset, -1, "Object ID");
                ctree = proto_item_add_subtree(citem, ett_ndps);
                foffset = objectidentification(tvb, ctree, foffset);
                proto_item_set_end(citem, tvb, foffset);
                citem = proto_tree_add_text(btree, tvb, foffset, -1, "Event Object ID");
                ctree = proto_item_add_subtree(citem, ett_ndps);
                foffset = objectidentifier(tvb, ctree, foffset);
                proto_item_set_end(citem, tvb, foffset);
                /
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(btree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
                foffset += 4;
                citem = proto_tree_add_text(btree, tvb, foffset, -1, "Attribute Modifications");
                ctree = proto_item_add_subtree(citem, ett_ndps);
                for (j = 1 ; j <= number_of_items; j++ )
                {
	            if (j > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(ctree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    ditem = proto_tree_add_text(ctree, tvb, foffset, -1, "Modification %d", j);
                    dtree = proto_item_add_subtree(ditem, ett_ndps);
                    foffset = attribute_value(tvb, dtree, foffset);  /
                    proto_item_set_end(ditem, tvb, foffset);
                }
                proto_item_set_end(citem, tvb, foffset);
                /
                foffset = ndps_string(tvb, hf_ndps_message, btree, foffset, NULL, 0);
                proto_tree_add_item(btree, hf_time, tvb, foffset, 4, FALSE);
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
            proto_tree_add_uint(ndps_tree, hf_ndps_num_destinations, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Destination %d", i);
                atree = proto_item_add_subtree(aitem, ett_ndps);
                /
                /
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Method ID");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = name_or_id(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                /
                /
                proto_tree_add_item(atree, hf_address_len, tvb, foffset, 4, FALSE);
                foffset += 4;
                foffset = print_address(tvb, atree, foffset);
                /
                proto_item_set_end(aitem, tvb, foffset);
                /
            }
            /
            foffset = ndps_string(tvb, hf_ndps_supplier_name, ndps_tree, foffset, NULL, 0);
            proto_tree_add_item(ndps_tree, hf_ndps_event_type, tvb, foffset, 4, FALSE);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Containing Class");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Containing Object");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentification(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Filter Class");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Object Class");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Object ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentification(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Event Object ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Attributes");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Attribute %d", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = attribute_value(tvb, btree, foffset);  
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = ndps_string(tvb, hf_ndps_message, ndps_tree, foffset, NULL, 0);
            proto_tree_add_item(ndps_tree, hf_time, tvb, foffset, 4, FALSE);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Account");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = qualifiedname(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            break;
        case 0x0000000c:    /
            foffset = ndps_string(tvb, hf_ndps_file_name, ndps_tree, foffset, NULL, 0);
            break;
        case 0x0000000d:    /
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Method ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            break;
        case 0x0000000e:    /
            cred_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_item(ndps_tree, hf_delivery_method_type, tvb, foffset, 4, FALSE);
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
                    proto_tree_add_item(ndps_tree, hf_ndps_integer_type_value, tvb, foffset, 4, FALSE);
                    foffset += 4;
                }
                /
                proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, FALSE);
                foffset += 4;
                break;
            case 1:       /
                length = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_context_len, tvb, foffset, 4, length);
                foffset += 4;
                if (length!=0)
                {
                    tvb_ensure_bytes_exist(tvb, foffset, length);
                    proto_tree_add_item(ndps_tree, hf_ndps_context, tvb, foffset, length, FALSE);
                }
                foffset += length;
                foffset += (length%2);
                proto_tree_add_item(ndps_tree, hf_ndps_abort_flag, tvb, foffset, 4, FALSE);
                foffset += 4;
                break;
            default:
                break;
            }
            break;
        case 0x0000000f:    /
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Method ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, FALSE);
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
            foffset = credentials(tvb, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_retrieve_restrictions, tvb, foffset, 4, FALSE);
            foffset += 4;
            number_of_items=tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_bind_security_option_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Security %d", i);
                atree = proto_item_add_subtree(aitem, ett_ndps);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(atree, hf_bind_security, tvb, foffset, length, FALSE);
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
            proto_tree_add_item(ndps_tree, hf_packet_count, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_last_packet_flag, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_file_timestamp, tvb, foffset, 4, FALSE);
            foffset += 4;
            foffset = res_add_input_data(tvb, ndps_tree, foffset);
            number_of_items=tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Item %d", i);
                atree = proto_item_add_subtree(aitem, ett_ndps);
                length=tvb_get_ntohl(tvb, foffset);
                length_remaining = tvb_length_remaining(tvb, foffset);
                if(length_remaining == -1 || (guint32) length_remaining < length)
                {
                    return;
                }
                proto_tree_add_item(atree, hf_ndps_item_ptr, tvb, foffset, length, FALSE);
                foffset += length;
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 0x00000004:    /
            foffset = res_add_input_data(tvb, ndps_tree, foffset);
            break;
        case 0x00000005:    /
            proto_tree_add_item(ndps_tree, hf_ndps_max_items, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_status_flags, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_resource_list_type, tvb, foffset, 4, FALSE);
            resource_type = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            switch (resource_type) 
            {
            case 0:     /
                proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                break;
            case 1:     /
            case 2:     /
                foffset = ndps_string(tvb, hf_ndps_vendor_dir, ndps_tree, foffset, NULL, 0);
                break;
            case 3:     /
                proto_tree_add_item(ndps_tree, hf_banner_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                break;
            case 4:     /
                proto_tree_add_item(ndps_tree, hf_font_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                break;
            case 5:     /
            case 12:    /
            case 9:     /
                proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                foffset = ndps_string(tvb, hf_ndps_printer_type, ndps_tree, foffset, NULL, 0);
                foffset = ndps_string(tvb, hf_ndps_printer_manuf, ndps_tree, foffset, NULL, 0);
                foffset = ndps_string(tvb, hf_ndps_inf_file_name, ndps_tree, foffset, NULL, 0);
                field_len = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                proto_tree_add_item(ndps_tree, hf_printer_id, tvb, foffset, field_len, FALSE);
                break;
            case 6:     /
            case 10:    /
                foffset = ndps_string(tvb, hf_ndps_vendor_dir, ndps_tree, foffset, NULL, 0);
                foffset += 4;
                foffset = ndps_string(tvb, hf_ndps_printer_type, ndps_tree, foffset, NULL, 0);
                foffset = ndps_string(tvb, hf_ndps_printer_manuf, ndps_tree, foffset, NULL, 0);
                foffset = ndps_string(tvb, hf_ndps_inf_file_name, ndps_tree, foffset, NULL, 0);
                field_len = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                proto_tree_add_item(ndps_tree, hf_printer_id, tvb, foffset, field_len, FALSE);
                break;
            case 7:     /
                proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_tree_add_item(ndps_tree, hf_font_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                foffset = ndps_string(tvb, hf_ndps_font_name, ndps_tree, foffset, NULL, 0);
                break;
            case 8:     /
            case 11:    /
            case 13:    /
                foffset = ndps_string(tvb, hf_ndps_printer_manuf, ndps_tree, foffset, NULL, 0);
                foffset = ndps_string(tvb, hf_ndps_printer_type, ndps_tree, foffset, NULL, 0);
                foffset = ndps_string(tvb, hf_ndps_inf_file_name, ndps_tree, foffset, NULL, 0);
                break;
            case 14:    /
                break;
            default:
                break;
            }
            break;
        case 0x00000006:    /
            proto_tree_add_item(ndps_tree, hf_get_status_flag, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_res_type, tvb, foffset, 4, FALSE);
            resource_type = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            switch (resource_type) 
            {
            case 0:     /
                proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                foffset = ndps_string(tvb, hf_ndps_prn_dir_name, ndps_tree, foffset, NULL, 0);
                foffset = ndps_string(tvb, hf_ndps_prn_file_name, ndps_tree, foffset, NULL, 0);
                break;
            case 1:     /
                foffset = ndps_string(tvb, hf_ndps_vendor_dir, ndps_tree, foffset, NULL, 0);
                foffset = ndps_string(tvb, hf_ndps_prn_file_name, ndps_tree, foffset, NULL, 0);
                break;
            case 2:     /
                foffset = ndps_string(tvb, hf_ndps_banner_name, ndps_tree, foffset, NULL, 0);
                break;
            case 3:     /
                proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_tree_add_item(ndps_tree, hf_font_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                foffset = ndps_string(tvb, hf_ndps_prn_file_name, ndps_tree, foffset, NULL, 0);
                break;
            case 4:     /
            case 5:     /
                proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                foffset = ndps_string(tvb, hf_ndps_prn_dir_name, ndps_tree, foffset, NULL, 0);
                proto_tree_add_item(ndps_tree, hf_archive_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                break;
            default:
                break;
            }
            break;
        case 0x00000007:    /
            proto_tree_add_item(ndps_tree, hf_ndps_status_flags, tvb, foffset, 4, FALSE);
            foffset += 4;
            foffset = res_add_input_data(tvb, ndps_tree, foffset);
            break;
        case 0x0000000a:    /
            proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, FALSE);
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
            foffset = credentials(tvb, ndps_tree, foffset);
            break;
        case 0x00000002:    /
            /
            break;
        case 0x00000003:    /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Item %d", i);
                atree = proto_item_add_subtree(aitem, ett_ndps);
                proto_tree_add_item(atree, hf_ndps_session, tvb, foffset, 4, FALSE);
                foffset += 4;
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Supplier ID");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(btree, hf_ndps_attribute_value, tvb, foffset, length, FALSE);
                }
                foffset += length;
                proto_tree_add_item(btree, hf_ndps_event_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_item_set_end(bitem, tvb, foffset);
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Containing Class");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Containing Object");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = objectidentification(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Filter Class");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Object Class");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Object ID");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = objectidentification(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Event Object ID");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = objectidentifier(tvb, btree, foffset);
                foffset = attribute_value(tvb, atree, foffset);
                foffset = ndps_string(tvb, hf_ndps_message, atree, foffset, NULL, 0);
                proto_tree_add_item(atree, hf_time, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_item_set_end(bitem, tvb, foffset);
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Account");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = qualifiedname(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 0x00000004:    /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Object %d", i);
                atree = proto_item_add_subtree(aitem, ett_ndps);
                proto_tree_add_item(atree, hf_ndps_session, tvb, foffset, 4, FALSE);
                foffset += 4;
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Supplier ID");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(btree, hf_ndps_attribute_value, tvb, foffset, length, FALSE);
                }
                foffset += length;
                proto_tree_add_item(atree, hf_ndps_event_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_item_set_end(bitem, tvb, foffset);
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Containing Class");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Containing Object");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = objectidentification(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Filter Class");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Object Class");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Object ID");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = objectidentification(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Event Object ID");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                /
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
                foffset += 4;
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Attribute");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(btree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    foffset = attribute_value(tvb, btree, foffset);  
                }
                proto_item_set_end(bitem, tvb, foffset);
                /
                foffset = ndps_string(tvb, hf_ndps_message, atree, foffset, NULL, 0);
                proto_tree_add_item(atree, hf_time, tvb, foffset, 4, FALSE);
                foffset += 4;
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Account");
                btree = proto_item_add_subtree(bitem, ett_ndps);
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
    return;
}
