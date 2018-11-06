static void
dissect_ndps_reply(tvbuff_t *tvb, packet_info *pinfo, proto_tree *ndps_tree, int foffset)
{
    conversation_t          *conversation = NULL;
    ndps_req_hash_value     *request_value = NULL;
    proto_tree              *atree;
    proto_item              *aitem;
    proto_tree              *btree;
    proto_item              *bitem;
    proto_tree              *ctree;
    proto_item              *citem;
    proto_tree              *dtree;
    proto_item              *ditem;
    guint32                 i;
    guint32                 j;
    guint32                 k;
    guint32                 number_of_items=0;
    guint32                 number_of_items2=0;
    guint32                 number_of_items3=0;
    guint32                 length=0;
    guint32                 ndps_func=0;
    guint32                 ndps_prog=0;
    guint32                 error_val=0;
    guint32                 resource_type=0;
    gint		    length_remaining;
    
    if (!pinfo->fd->flags.visited) {
        /
        conversation = find_conversation(pinfo->fd->num, &pinfo->src, &pinfo->dst,
            PT_NCP, (guint32) pinfo->destport, (guint32) pinfo->destport, 0);
        if (conversation != NULL) {
            /
            request_value = ndps_hash_lookup(conversation, (guint32) pinfo->destport);
            p_add_proto_data(pinfo->fd, proto_ndps, (void*) request_value);
        }
        /
    }
    else {
        request_value = p_get_proto_data(pinfo->fd, proto_ndps);
    }
    if (request_value) {
        ndps_prog = request_value->ndps_prog;
        ndps_func = request_value->ndps_func;
        proto_tree_add_uint_format(ndps_tree, hf_ndps_reqframe, tvb, 0, 
           0, request_value->ndps_frame_num,
           "Response to Request in Frame Number: %u",
           request_value->ndps_frame_num);
    }

    if (tvb_length_remaining(tvb, foffset) < 12 && tvb_get_ntohl(tvb, foffset) == 0) /
    {
        proto_tree_add_uint(ndps_tree, hf_ndps_error_val, tvb, foffset, 4, error_val);
        if (check_col(pinfo->cinfo, COL_INFO))
                col_append_str(pinfo->cinfo, COL_INFO, "- Ok");
        return;
    }
    if(ndps_func == 1 || ndps_func == 2)
    {
        expert_item = proto_tree_add_item(ndps_tree, hf_ndps_rpc_acc_stat, tvb, foffset, 4, FALSE);
        expert_status = tvb_get_ntohl(tvb, foffset);
        if (expert_status != 0) {
            expert_add_info_format(pinfo, expert_item, PI_RESPONSE_CODE, PI_ERROR, "Fault: %s", val_to_str(expert_status, accept_stat, "Unknown NDPS Error (0x%08x)"));
        }
        foffset += 4;
        if (tvb_length_remaining(tvb,foffset) < 4 ) {
            if (check_col(pinfo->cinfo, COL_INFO))
                col_append_str(pinfo->cinfo, COL_INFO, "- Error");
            return;
        }
        proto_tree_add_item(ndps_tree, hf_ndps_rpc_acc_results, tvb, foffset, 4, FALSE);
        foffset += 4;
        if (tvb_length_remaining(tvb,foffset) < 4) {
            if (check_col(pinfo->cinfo, COL_INFO))
                col_append_str(pinfo->cinfo, COL_INFO, "- Error");
            return;
        }
    }
    error_val = tvb_get_ntohl(tvb, foffset);
    proto_tree_add_uint(ndps_tree, hf_ndps_error_val, tvb, foffset, 4, error_val);
    foffset += 4;
    /
    if (match_strval(tvb_get_ntohl(tvb, foffset), ndps_error_types) && tvb_length_remaining(tvb,foffset) < 8 && (tvb_get_ntohl(tvb, foffset)!=0))  
    {
        expert_status = tvb_get_ntohl(tvb, foffset);
        expert_item = proto_tree_add_item(ndps_tree, hf_ndps_return_code, tvb, foffset, 4, FALSE);
        expert_add_info_format(pinfo, expert_item, PI_RESPONSE_CODE, PI_ERROR, "Fault: %s", val_to_str(expert_status, ndps_error_types, "Unknown NDPS Error (0x%08x)"));
        if (check_col(pinfo->cinfo, COL_INFO))
            col_append_str(pinfo->cinfo, COL_INFO, "- Error");
        return;
    }
    if (check_col(pinfo->cinfo, COL_INFO))
        col_append_str(pinfo->cinfo, COL_INFO, "- Ok");
    switch(ndps_prog)
    {
    case 0x060976:  /
        switch(ndps_func)
        {
        case 0x00000001:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            if(error_val != 0)
            {
                foffset = ndps_error(tvb, pinfo, ndps_tree, foffset);
                if(tvb_length_remaining(tvb, foffset) < 4)
                {
                    break;
                }
                proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
                foffset += 4;
            }
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "PSM Name");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = qualifiedname(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            break;
        case 0x00000002:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            if(error_val != 0)
            {
                foffset = ndps_error(tvb, pinfo, ndps_tree, foffset);
                if(tvb_length_remaining(tvb, foffset) < 4)
                {
                    break;
                }
                proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
                foffset += 4;
            }
                foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL, 0);
            break;
        case 0x00000003:    /
            break;
        case 0x00000004:    /
            foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL, 0);
            proto_tree_add_item(ndps_tree, hf_local_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            if(error_val != 0)
            {
                foffset = ndps_error(tvb, pinfo, ndps_tree, foffset);
            }
            break;
        case 0x00000005:    /
        case 0x00000006:    /
        case 0x00000008:    /
        case 0x0000000b:    /
        case 0x0000000d:    /
            /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Object Attribute Set");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Attribute %d", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = attribute_value(tvb, btree, foffset);  /
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            /
            if(error_val != 0)
            {
                foffset = ndps_error(tvb, pinfo, ndps_tree, foffset);
            }
            break;
        case 0x00000007:    /
            proto_tree_add_item(ndps_tree, hf_answer_time, tvb, foffset, 4, FALSE);
            foffset += 4;
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Continuation Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items=tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(atree, hf_ndps_num_options, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
		    proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
		    break;
		}
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Option %d", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                length=tvb_get_ntohl(tvb, foffset);
                length_remaining = tvb_length_remaining(tvb, foffset);
                if(length_remaining == -1 || (guint32) length_remaining < length)
                {
                    return;
                }
                proto_tree_add_item(btree, hf_ndps_item_ptr, tvb, foffset, length, FALSE);
                foffset += length;
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Limit Encountered Option");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            proto_tree_add_item(atree, hf_ndps_len, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(atree, hf_limit_enc, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_item_set_end(aitem, tvb, foffset);
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Object Results Set");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items=tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(atree, hf_ndps_num_results, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Result %d", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = objectidentification(tvb, btree, foffset);
                number_of_items2 = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(btree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items2);
                foffset += 4;
                for (j = 1 ; j <= number_of_items2; j++ )
                {
	            if (j > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(btree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    citem = proto_tree_add_text(btree, tvb, foffset, -1, "Attribute %d", j);
                    ctree = proto_item_add_subtree(citem, ett_ndps);
                    foffset = objectidentifier(tvb, ctree, foffset);
                    foffset += align_4(tvb, foffset);
                    number_of_items3 = tvb_get_ntohl(tvb, foffset);
                    proto_tree_add_uint(ctree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items3);
                    foffset += 4;
                    for (k = 1 ; k <= number_of_items3; k++ )
                    {
	                if (k > NDPS_MAX_ITEMS) {
	                    proto_tree_add_text(ctree, tvb, foffset, -1, "[Truncated]");
	                    break;
	                }
                        ditem = proto_tree_add_text(ctree, tvb, foffset, -1, "Value %d", k);
                        dtree = proto_item_add_subtree(ditem, ett_ndps);
                        foffset = attribute_value(tvb, dtree, foffset);
                        foffset += align_4(tvb, foffset);
                        proto_item_set_end(ditem, tvb, foffset);
                    }
                    proto_tree_add_item(ctree, hf_ndps_qualifier, tvb, foffset, 4, FALSE);
                    foffset += 4;
                    proto_item_set_end(citem, tvb, foffset);
                }
                foffset = objectidentifier(tvb, btree, foffset);
                foffset += 2;
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            if(error_val != 0)
            {
                foffset = ndps_error(tvb, pinfo, ndps_tree, foffset);
            }
            break;
        case 0x00000009:    /
        case 0x0000000a:    /
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Job ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = ndps_string(tvb, hf_ndps_pa_name, atree, foffset, NULL, 0);
            proto_tree_add_item(atree, hf_local_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_item_set_end(aitem, tvb, foffset);
            /
            /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Object Attribute Set");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Attribute %d", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = attribute_value(tvb, btree, foffset);  /
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            /
            if(error_val != 0)
            {
                foffset = ndps_error(tvb, pinfo, ndps_tree, foffset);
            }
            break;
        case 0x0000000c:    /
        case 0x0000000e:    /
        case 0x0000000f:    /
        case 0x00000010:    /
        case 0x00000012:    /
        case 0x00000013:    /
        case 0x00000014:    /
        case 0x00000018:    /
        case 0x00000019:    /
        case 0x0000001b:    /
        case 0x0000001c:    /
        case 0x0000001e:    /
        case 0x0000001f:    /
        case 0x00000020:    /
        case 0x00000021:    /
            if(error_val != 0)
            {
                foffset = ndps_error(tvb, pinfo, ndps_tree, foffset);
            }
            break;
        case 0x00000011:    /
            number_of_items = tvb_get_ntohl(tvb, foffset); /
            proto_tree_add_uint(ndps_tree, hf_ndps_num_jobs, tvb, foffset, 4, number_of_items);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Resubmit Job");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Job %d", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                /
                citem = proto_tree_add_text(btree, tvb, foffset, -1, "Old Job");
                ctree = proto_item_add_subtree(citem, ett_ndps);
                foffset = ndps_string(tvb, hf_ndps_pa_name, ctree, foffset, NULL, 0);
                proto_tree_add_item(ctree, hf_local_id, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_item_set_end(citem, tvb, foffset);
                /
                /
                citem = proto_tree_add_text(btree, tvb, foffset, -1, "New Job");
                ctree = proto_item_add_subtree(citem, ett_ndps);
                foffset = ndps_string(tvb, hf_ndps_pa_name, ctree, foffset, NULL, 0);
                proto_tree_add_item(ctree, hf_local_id, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_item_set_end(citem, tvb, foffset);
                /
                /
                number_of_items2 = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ctree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items2);
                foffset += 4;
                citem = proto_tree_add_text(btree, tvb, foffset, -1, "Job Status");
                ctree = proto_item_add_subtree(citem, ett_ndps);
                for (j = 1 ; j <= number_of_items2; j++ )
                {
	            if (j > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(ctree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    ditem = proto_tree_add_text(ctree, tvb, foffset, -1, "Object %d", j);
                    dtree = proto_item_add_subtree(ditem, ett_ndps);
                    foffset = attribute_value(tvb, dtree, foffset);  /
                    proto_item_set_end(ditem, tvb, foffset);
                }
                proto_item_set_end(citem, tvb, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                /
            }
            proto_item_set_end(aitem, tvb, foffset);
            /
            if(error_val != 0)
            {
                foffset = ndps_error(tvb, pinfo, ndps_tree, foffset);
            }
            break;
        case 0x00000015:    /
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Job Status");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Object %d", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = attribute_value(tvb, btree, foffset);  /
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            /
            if(error_val != 0)
            {
                foffset = ndps_error(tvb, pinfo, ndps_tree, foffset);
            }
            break;
        case 0x00000016:    /
        case 0x00000017:    /
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Printer Status");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Object %d", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = attribute_value(tvb, atree, foffset);  /
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
            /
            if(error_val != 0)
            {
                foffset = ndps_error(tvb, pinfo, ndps_tree, foffset);
            }
            break;
        case 0x0000001a:    /
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            if(error_val != 0)
            {
                foffset = ndps_error(tvb, pinfo, ndps_tree, foffset);
            }
            break;
        case 0x0000001d:    /
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length==4)
            {
                proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, length, FALSE);
            }
            foffset += length;
            /
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_persistence, tvb, foffset, 4, FALSE);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Consumer Name");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = qualifiedname(tvb, atree, foffset);
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length==4)
            {
                proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, length, FALSE);
            }
            foffset += length;
            proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_item_set_end(aitem, tvb, foffset);
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Method ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Delivery Addresses");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_delivery_add_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
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
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length!=0)
            {
                tvb_ensure_bytes_exist(tvb, foffset, length);
                proto_tree_add_item(ndps_tree, hf_ndps_continuation_option, tvb, foffset, length, FALSE);
            }
            foffset += length;
            if(error_val != 0)
            {
                foffset = ndps_error(tvb, pinfo, ndps_tree, foffset);
            }
            break;
        case 0x00000022:    /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "NDS Printer Name");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = qualifiedname(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            if(error_val != 0)
            {
                foffset = ndps_error(tvb, pinfo, ndps_tree, foffset);
            }
            break;
        case 0x00000023:    /
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_notify_lease_exp_time, tvb, foffset, 4, FALSE);
            foffset += 4;
            if(error_val != 0)
            {
                foffset = ndps_error(tvb, pinfo, ndps_tree, foffset);
            }
            break;
        case 0x00000024:    /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_num_events, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Event %d", i);
                atree = proto_item_add_subtree(aitem, ett_ndps);
                /
                proto_tree_add_item(atree, hf_ndps_profile_id, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_tree_add_item(atree, hf_ndps_persistence, tvb, foffset, 4, FALSE);
                foffset += 4;
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Consumer Name");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = qualifiedname(tvb, btree, foffset);
                foffset = ndps_string(tvb, hf_ndps_supplier_name, atree, foffset, NULL, 0);
                proto_tree_add_item(atree, hf_ndps_language_id, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_item_set_end(bitem, tvb, foffset);
                /
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Method ID");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = name_or_id(tvb, btree, foffset);
                foffset += align_4(tvb, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                /
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_delivery_add_count, tvb, foffset, 4, number_of_items);
                foffset += 4;
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Delivery Addresses");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(btree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    citem = proto_tree_add_text(btree, tvb, foffset, -1, "Address %d", i);
                    ctree = proto_item_add_subtree(citem, ett_ndps);
                    foffset = address_item(tvb, ctree, foffset);
                    proto_item_set_end(citem, tvb, foffset);
                }
                proto_item_set_end(bitem, tvb, foffset);
                foffset = event_object_set(tvb, atree, foffset);
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Account");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = qualifiedname(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                /
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Notify Attributes");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(btree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(btree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    citem = proto_tree_add_text(btree, tvb, foffset, -1, "Attribute %d", i);
                    ctree = proto_item_add_subtree(citem, ett_ndps);
                    foffset = objectidentifier(tvb, ctree, foffset);
                    proto_item_set_end(citem, tvb, foffset);
                }
                proto_item_set_end(bitem, tvb, foffset);
                /
                proto_tree_add_item(atree, hf_notify_time_interval, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_tree_add_item(atree, hf_notify_sequence_number, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_tree_add_item(atree, hf_notify_lease_exp_time, tvb, foffset, 4, FALSE);
                foffset += 4;
                foffset = ndps_string(tvb, hf_notify_printer_uri, atree, foffset, NULL, 0);
                proto_item_set_end(aitem, tvb, foffset);
                /
                length = tvb_get_ntohl(tvb, foffset); /
                foffset += 4;
                if (length!=0)
                {
                    tvb_ensure_bytes_exist(tvb, foffset, length);
                    proto_tree_add_item(ndps_tree, hf_ndps_continuation_option, tvb, foffset, length, FALSE);
                }
                foffset += length;
                if(error_val != 0)
                {
                    foffset = ndps_error(tvb, pinfo, ndps_tree, foffset);
                }
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
        case 0x00000002:    /
        case 0x00000004:    /
        case 0x00000005:    /
        case 0x00000006:    /
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000003:    /
            number_of_items = tvb_get_ntohl(tvb, foffset); 
            proto_tree_add_uint(ndps_tree, hf_ndps_num_services, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Service %d", i);
                atree = proto_item_add_subtree(aitem, ett_ndps);
                proto_tree_add_item(atree, hf_ndps_service_type, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_tree_add_item(atree, hf_ndps_service_enabled, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_item_set_end(aitem, tvb, foffset);
            }
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000007:    /
            proto_tree_add_item(ndps_tree, hf_ndps_item_count, tvb, foffset,
            4, FALSE);	/
            foffset += 4;
            foffset = ndps_string(tvb, hf_ndps_broker_name, ndps_tree, foffset, NULL, 0);
            foffset = ndps_string(tvb, hf_ndps_tree, ndps_tree, foffset, NULL, 0);
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000008:    /
        default:
            break;
        }
        break;
    case 0x060978:  /
        switch(ndps_func)
        {
        case 0x00000001:    /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Attribute %d", i);
                atree = proto_item_add_subtree(aitem, ett_ndps);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length!=0)
                {
                    tvb_ensure_bytes_exist(tvb, foffset, length);
                    proto_tree_add_item(atree, hf_ndps_attribute_set, tvb, foffset, length, FALSE);
                }
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 0x00000002:    /
            /
            break;
        case 0x00000003:    /
        case 0x00000004:    /
        case 0x00000005:    /
        case 0x00000006:    /
        case 0x00000007:    /
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000008:    /
        case 0x00000009:    /
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
                foffset = server_entry(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
            }
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length!=0)
            {
                tvb_ensure_bytes_exist(tvb, foffset, length);
                proto_tree_add_item(ndps_tree, hf_ndps_continuation_option, tvb, foffset, length, FALSE);
            }
            foffset += length;
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x0000000a:    /
            number_of_items = tvb_get_ntohl(tvb, foffset); 
            proto_tree_add_item(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, FALSE);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                aitem = proto_tree_add_item(ndps_tree, hf_ndps_client_server_type, tvb, foffset, 4, FALSE);
                atree = proto_item_add_subtree(aitem, ett_ndps);
                foffset += 4;
                foffset = ndps_string(tvb, hf_ndps_registry_name, atree, foffset, NULL, 0);
                foffset = print_address(tvb, atree, foffset);
            }
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length!=0)
            {
                tvb_ensure_bytes_exist(tvb, foffset, length);
                proto_tree_add_item(ndps_tree, hf_ndps_continuation_option, tvb, foffset, length, FALSE);
            }
            foffset += length;
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x0000000b:    /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "NDS Printer Name");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = qualifiedname(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x0000000c:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session_type, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_time, tvb, foffset, 4, FALSE);
            foffset += 4;
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        default:
            break;
        }
        break;
    case 0x060979:  /
        switch(ndps_func)
        {
        case 0x00000001:    /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Attribute %d", i);
                atree = proto_item_add_subtree(aitem, ett_ndps);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length!=0)
                {
                    tvb_ensure_bytes_exist(tvb, foffset, length);
                    proto_tree_add_item(atree, hf_ndps_attribute_set, tvb, foffset, length, FALSE);
                }
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 0x00000002:    /
            /
            break;
        case 0x00000003:    /
            proto_tree_add_item(ndps_tree, hf_ndps_session, tvb, foffset, 4, FALSE);
            foffset += 4;
            foffset = event_object_set(tvb, ndps_tree, foffset);
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000004:    /
        case 0x0000000b:    /
        case 0x0000000d:    /
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000005:    /
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, FALSE);
            foffset += 4;
            foffset = event_object_set(tvb, ndps_tree, foffset);
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000006:    /
        case 0x00000007:    /
        case 0x00000009:    /
            foffset = event_object_set(tvb, ndps_tree, foffset);
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000008:    /
            /
            proto_tree_add_item(ndps_tree, hf_ndps_len, tvb, foffset, 4, FALSE);
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
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Delivery Addresses");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_delivery_add_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
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
            /
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length!=0)
            {
                tvb_ensure_bytes_exist(tvb, foffset, length);
                proto_tree_add_item(ndps_tree, hf_ndps_continuation_option, tvb, foffset, length, FALSE);
            }
            foffset += length;
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x0000000a:    /
            /
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length==4)
            {
                proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, length, FALSE);
            }
            foffset += length;
            /
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x0000000c:    /
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Method ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x0000000e:    /
            /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_delivery_method_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                /
                aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Method %d", i);
                atree = proto_item_add_subtree(aitem, ett_ndps);
                /
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Method ID");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = name_or_id(tvb, btree, foffset);
                foffset += align_4(tvb, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                /
                foffset = ndps_string(tvb, hf_ndps_method_name, atree, foffset, NULL, 0);
                foffset = ndps_string(tvb, hf_ndps_method_ver, atree, foffset, NULL, 0);
                foffset = ndps_string(tvb, hf_ndps_file_name, atree, foffset, NULL, 0);
                proto_tree_add_item(atree, hf_ndps_admin_submit, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_item_set_end(aitem, tvb, foffset);
                /
            }
            /
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x0000000f:    /
            /
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Method ID");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = name_or_id(tvb, atree, foffset);
            /
            foffset = ndps_string(tvb, hf_ndps_method_name, atree, foffset, NULL, 0);
            foffset = ndps_string(tvb, hf_ndps_method_ver, atree, foffset, NULL, 0);
            foffset = ndps_string(tvb, hf_ndps_file_name, atree, foffset, NULL, 0);
            proto_tree_add_item(atree, hf_ndps_admin_submit, tvb, foffset, 4, FALSE);
            foffset += 4;
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
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000010:    /
            proto_tree_add_item(ndps_tree, hf_ndps_item_count, tvb, foffset,
            4, FALSE);	/
            foffset += 4;
            foffset = ndps_string(tvb, hf_ndps_broker_name, ndps_tree, foffset, NULL, 0);
            foffset = ndps_string(tvb, hf_ndps_tree, ndps_tree, foffset, NULL, 0);
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000011:    /
            proto_tree_add_item(ndps_tree, hf_ndps_get_session_type, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_time, tvb, foffset, 4, FALSE);
            foffset += 4;
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        default:
            break;
        }
        break;
    case 0x06097a:  /
        switch(ndps_func)
        {
        case 0x00000001:    /
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length!=0)
            {
                tvb_ensure_bytes_exist(tvb, foffset, length);
                proto_tree_add_item(ndps_tree, hf_ndps_attribute_set, tvb, foffset, length, FALSE);
            }
            break;
        case 0x00000002:    /
            /
            break;
        case 0x00000003:    /
        case 0x00000004:    /
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x00000005:    /
            proto_tree_add_item(ndps_tree, hf_ndps_return_code, tvb, foffset, 4, FALSE);
            foffset += 4;
            if (check_col(pinfo->cinfo, COL_INFO) && tvb_get_ntohl(tvb, foffset-4) != 0)
                col_add_fstr(pinfo->cinfo, COL_INFO, "R NDPS - Error");
            if (tvb_get_ntohl(tvb, foffset-4) != 0) 
            {
                break;
            }
            proto_tree_add_item(ndps_tree, hf_ndps_status_flags, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_resource_list_type, tvb, foffset, 4, FALSE);
            resource_type = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            switch (resource_type) 
            {
            case 0:     /
            case 1:     /
            case 2:     /
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_printer_def_count, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Definition %d", i);
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    if (tvb_get_ntohl(tvb, foffset)==0) {  /
                        foffset += 2;
                    }
                    foffset += 4; /
                    foffset = ndps_string(tvb, hf_ndps_printer_manuf, atree, foffset, NULL, 0);
                    if (tvb_get_ntohl(tvb, foffset)==0) {
                        foffset += 2;
                    }
                    foffset += 4;
                    foffset = ndps_string(tvb, hf_ndps_printer_type, atree, foffset, NULL, 0);
                    if (tvb_get_ntohl(tvb, foffset)==0) {
                        foffset += 2;
                    }
                    foffset += 4;
                    foffset = ndps_string(tvb, hf_ndps_inf_file_name, atree, foffset, NULL, 0);
                    proto_item_set_end(aitem, tvb, foffset);
                }
                break;
            case 3:     /
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Banner %d", i);
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    foffset = ndps_string(tvb, hf_ndps_banner_name, atree, foffset, NULL, 0);
                    proto_item_set_end(aitem, tvb, foffset);
                }
                break;
            case 4:     /
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_font_type_count, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Font %d", i);
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    foffset = ndps_string(tvb, hf_font_type_name, atree, foffset, NULL, 0);
                    proto_item_set_end(aitem, tvb, foffset);
                }
                break;
            case 7:     /
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_font_file_count, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Font File %d", i);
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    foffset = ndps_string(tvb, hf_font_file_name, atree, foffset, NULL, 0);
                    proto_item_set_end(aitem, tvb, foffset);
                }
                break;
            case 5:     /
            case 12:    /
            case 9:     /
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_printer_def_count, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "File %d", i);
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    foffset = ndps_string(tvb, hf_ndps_prn_file_name, atree, foffset, NULL, 0);
                    foffset = ndps_string(tvb, hf_ndps_prn_dir_name, atree, foffset, NULL, 0);
                    foffset = ndps_string(tvb, hf_ndps_inf_file_name, atree, foffset, NULL, 0);
                    proto_item_set_end(aitem, tvb, foffset);
                }
                break;
            case 6:     /
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_printer_def_count, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Definition %d", i);
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    foffset = ndps_string(tvb, hf_ndps_prn_file_name, atree, foffset, NULL, 0);
                    foffset = ndps_string(tvb, hf_ndps_prn_dir_name, atree, foffset, NULL, 0);
                    foffset = ndps_string(tvb, hf_ndps_inf_file_name, atree, foffset, NULL, 0);
                    proto_item_set_end(aitem, tvb, foffset);
                }
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
                    foffset = ndps_string(tvb, hf_ndps_def_file_name, atree, foffset, NULL, 0);
                    number_of_items2 = tvb_get_ntohl(tvb, foffset);
                    proto_tree_add_uint(atree, hf_ndps_num_win31_keys, tvb, foffset, 4, number_of_items2);
                    bitem = proto_tree_add_text(atree, tvb, foffset, 4, "Windows 3.1 Keys");
                    btree = proto_item_add_subtree(bitem, ett_ndps);
                    foffset += 4;
                    for (i = 1 ; i <= number_of_items2; i++ )
                    {
	                if (i > NDPS_MAX_ITEMS) {
	                    proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	                    break;
	                }
                        foffset = ndps_string(tvb, hf_ndps_windows_key, btree, foffset, NULL, 0);
                    }
                    proto_item_set_end(bitem, tvb, foffset);
                    number_of_items2 = tvb_get_ntohl(tvb, foffset);
                    proto_tree_add_uint(atree, hf_ndps_num_win95_keys, tvb, foffset, 4, number_of_items2);
                    bitem = proto_tree_add_text(atree, tvb, foffset, 4, "Windows 95 Keys");
                    btree = proto_item_add_subtree(bitem, ett_ndps);
                    foffset += 4;
                    for (i = 1 ; i <= number_of_items2; i++ )
                    {
	                if (i > NDPS_MAX_ITEMS) {
	                    proto_tree_add_text(btree, tvb, foffset, -1, "[Truncated]");
	                    break;
	                }
                        foffset = ndps_string(tvb, hf_ndps_windows_key, btree, foffset, NULL, 0);
                    }
                    proto_item_set_end(bitem, tvb, foffset);
                    proto_item_set_end(aitem, tvb, foffset);
                }
                break;
            case 10:    /
                foffset = ndps_string(tvb, hf_ndps_def_file_name, ndps_tree, foffset, NULL, 0);
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_os_count, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "OS %d", i);
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    proto_tree_add_item(atree, hf_os_type, tvb, foffset, 4, FALSE);
                    foffset += 4;
                    number_of_items2 = tvb_get_ntohl(tvb, foffset);
                    proto_tree_add_uint(atree, hf_ndps_num_windows_keys, tvb, foffset, 4, number_of_items2);
                    foffset += 4;
                    for (i = 1 ; i <= number_of_items2; i++ )
                    {
	                if (i > NDPS_MAX_ITEMS) {
	                    proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	                    break;
	                }
                        bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Key %d", i);
                        btree = proto_item_add_subtree(bitem, ett_ndps);
                        foffset = ndps_string(tvb, hf_ndps_windows_key, btree, foffset, NULL, 0);
                        proto_item_set_end(bitem, tvb, foffset);
                    }
                    proto_item_set_end(aitem, tvb, foffset);
                }
                break;
            case 8:     /
            case 11:    /
            case 13:    /
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_printer_type_count, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Type %d", i);
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    foffset = ndps_string(tvb, hf_ndps_printer_manuf, atree, foffset, NULL, 0);
                    foffset = ndps_string(tvb, hf_ndps_printer_type, atree, foffset, NULL, 0);
                    foffset = ndps_string(tvb, hf_ndps_prn_file_name, atree, foffset, NULL, 0);
                    foffset = ndps_string(tvb, hf_ndps_prn_dir_name, atree, foffset, NULL, 0);
                    proto_tree_add_item(atree, hf_archive_type, tvb, foffset, 4, FALSE);
                    foffset += 4;
                    proto_tree_add_item(atree, hf_archive_file_size, tvb, foffset, 4, FALSE);
                    foffset += 4;
                    proto_item_set_end(aitem, tvb, foffset);
                }
                break;
            case 14:    /
                number_of_items = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_language_count, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (i = 1 ; i <= number_of_items; i++ )
                {
	            if (i > NDPS_MAX_ITEMS) {
	                proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	                break;
	            }
                    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Language %d", i);
                    atree = proto_item_add_subtree(aitem, ett_ndps);
                    proto_tree_add_item(atree, hf_ndps_language_id, tvb, foffset, 4, FALSE);
                    foffset += 4;
                    proto_item_set_end(aitem, tvb, foffset);
                }
                break;
            default:
                break;
            }
            break;
        case 0x00000006:    /
            proto_tree_add_item(ndps_tree, hf_ndps_return_code, tvb, foffset, 4, FALSE);
            foffset += 4;
            if (check_col(pinfo->cinfo, COL_INFO) && tvb_get_ntohl(tvb, foffset-4) != 0)
                col_add_fstr(pinfo->cinfo, COL_INFO, "R NDPS - Error");
            if (tvb_get_ntohl(tvb, foffset-4) != 0) 
            {
                break;
            }
            proto_tree_add_item(ndps_tree, hf_get_status_flag, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_file_timestamp, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_data, tvb, foffset, -1, FALSE);
            break;
        case 0x00000007:    /
            proto_tree_add_item(ndps_tree, hf_ndps_return_code, tvb, foffset, 4, FALSE);
            foffset += 4;
            if (check_col(pinfo->cinfo, COL_INFO) && tvb_get_ntohl(tvb, foffset-4) != 0)
                col_add_fstr(pinfo->cinfo, COL_INFO, "R NDPS - Error");
            if (tvb_get_ntohl(tvb, foffset-4) != 0) 
            {
                break;
            }
            proto_tree_add_item(ndps_tree, hf_file_timestamp, tvb, foffset, 4, FALSE);
            foffset += 4;
            break;
        case 0x00000008:    /
            foffset = qualifiedname(tvb, ndps_tree, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_error_val, tvb, foffset, 4, error_val);
            foffset += 4;
            break;
        case 0x00000009:    /
            proto_tree_add_item(ndps_tree, hf_ndps_get_resman_session_type, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_time, tvb, foffset, 4, FALSE);
            foffset += 4;
            foffset = return_code(tvb, pinfo, ndps_tree, foffset);
            break;
        case 0x0000000a:    /
            proto_tree_add_item(ndps_tree, hf_ndps_return_code, tvb, foffset, 4, FALSE);
            foffset += 4;
            if (check_col(pinfo->cinfo, COL_INFO) && tvb_get_ntohl(tvb, foffset-4) != 0)
                col_add_fstr(pinfo->cinfo, COL_INFO, "R NDPS - Error");
            if (tvb_get_ntohl(tvb, foffset-4) != 0) 
            {
                break;
            }
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
            proto_tree_add_item(ndps_tree, hf_ndps_return_code, tvb, foffset, 4, FALSE);
            foffset += 4;
            if (check_col(pinfo->cinfo, COL_INFO) && tvb_get_ntohl(tvb, foffset-4) != 0)
                col_add_fstr(pinfo->cinfo, COL_INFO, "R NDPS - Error");
            break;
        case 0x00000002:    /
            /
            break;
        case 0x00000003:    /
        case 0x00000004:    /
            proto_tree_add_item(ndps_tree, hf_ndps_return_code, tvb, foffset, 4, FALSE);
            foffset += 4;
            if (check_col(pinfo->cinfo, COL_INFO) && tvb_get_ntohl(tvb, foffset-4) != 0)
                col_add_fstr(pinfo->cinfo, COL_INFO, "R NDPS - Error");
            if (tvb_get_ntohl(tvb, foffset-4) != 0) 
            {
                break;
            }
            number_of_items=tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Failed Items");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            for (i = 1 ; i <= number_of_items; i++ )
            {
	        if (i > NDPS_MAX_ITEMS) {
	            proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	            break;
	        }
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Item %d", i);
                btree = proto_item_add_subtree(bitem, ett_ndps);
                length=tvb_get_ntohl(tvb, foffset);
                length_remaining = tvb_length_remaining(tvb, foffset);
                if(length_remaining == -1 || (guint32) length_remaining < length)
                {
                    return;
                }
                proto_tree_add_item(btree, hf_ndps_item_ptr, tvb, foffset, length, FALSE);
                foffset += length;
                proto_item_set_end(bitem, tvb, foffset);
            }
            proto_item_set_end(aitem, tvb, foffset);
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
