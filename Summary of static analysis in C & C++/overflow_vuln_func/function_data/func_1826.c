static int
ndps_error(tvbuff_t *tvb, packet_info *pinfo, proto_tree *ndps_tree, int foffset)
{
    guint32     number_of_items;
    guint32     ndps_problem_type;
    guint32     problem_type;
    guint32     i;
    proto_tree  *atree;
    proto_item  *aitem;
    proto_tree  *btree;
    proto_item  *bitem;

    ndps_problem_type = tvb_get_ntohl(tvb, foffset);
    if (check_col(pinfo->cinfo, COL_INFO))
        col_add_fstr(pinfo->cinfo, COL_INFO, "R NDPS - Error");
    expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_problem_type, tvb, foffset, 4, ndps_problem_type);
    expert_add_info_format(pinfo, expert_item, PI_RESPONSE_CODE, PI_ERROR, "Fault: %s", val_to_str(ndps_problem_type, error_type_enum, "Unknown NDPS Error (0x%08x)"));
    foffset += 4;
    switch(ndps_problem_type)
    {
    case 0:                 /
        problem_type = tvb_get_ntohl(tvb, foffset);
        proto_tree_add_uint(ndps_tree, hf_problem_type, tvb, foffset, 4, problem_type);
        foffset += 4;
        if (problem_type==0) /
        {
            proto_tree_add_item(ndps_tree, hf_security_problem_type, tvb, foffset, 4, FALSE);
            foffset += 4;
        }
        else                /
        {
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Extended Error");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
        }
        /
        aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Message");
        atree = proto_item_add_subtree(aitem, ett_ndps);
        foffset = name_or_id(tvb, atree, foffset);
        proto_item_set_end(aitem, tvb, foffset);
        /
        break;
    case 1:                 /
        proto_tree_add_item(ndps_tree, hf_problem_type, tvb, foffset, 4, FALSE);
        foffset += 4;
        if (tvb_get_ntohl(tvb, foffset-4)==0) /
        {
            proto_tree_add_item(ndps_tree, hf_service_problem_type, tvb, foffset, 4, FALSE);
            foffset += 4;
        }
        else                /
        {
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Extended Error");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
        }
        foffset = objectidentification(tvb, ndps_tree, foffset);
        foffset = attribute_value(tvb, ndps_tree, foffset);  /
        proto_tree_add_item(ndps_tree, hf_ndps_lib_error, tvb, foffset, 4, FALSE);
        foffset += 4;
        proto_tree_add_item(ndps_tree, hf_ndps_other_error, tvb, foffset, 4, FALSE);
        foffset += 4;
        proto_tree_add_item(ndps_tree, hf_ndps_other_error_2, tvb, foffset, 4, FALSE);
        foffset += 4;
        foffset = ndps_string(tvb, hf_ndps_other_error_string, ndps_tree, foffset, NULL, 0);
        break;
    case 2:                 /
        proto_tree_add_item(ndps_tree, hf_problem_type, tvb, foffset, 4, FALSE);
        foffset += 4;
        if (tvb_get_ntohl(tvb, foffset-4)==0) /
        {
            proto_tree_add_item(ndps_tree, hf_access_problem_type, tvb, foffset, 4, FALSE);
            foffset += 4;
        }
        else                /
        {
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Extended Error");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
        }
        foffset = objectidentification(tvb, ndps_tree, foffset);
        break;
    case 3:                 /
        proto_tree_add_item(ndps_tree, hf_problem_type, tvb, foffset, 4, FALSE);
        foffset += 4;
        if (tvb_get_ntohl(tvb, foffset-4)==0) /
        {
            proto_tree_add_item(ndps_tree, hf_printer_problem_type, tvb, foffset, 4, FALSE);
            foffset += 4;
        }
        else                /
        {
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Extended Error");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
        }
        foffset = objectidentification(tvb, ndps_tree, foffset);
        break;
    case 4:                 /
        proto_tree_add_item(ndps_tree, hf_problem_type, tvb, foffset, 4, FALSE);
        foffset += 4;
        if (tvb_get_ntohl(tvb, foffset-4)==0) /
        {
            proto_tree_add_item(ndps_tree, hf_selection_problem_type, tvb, foffset, 4, FALSE);
            foffset += 4;
        }
        else                /
        {
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Extended Error");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
        }
        foffset = objectidentification(tvb, ndps_tree, foffset);
        foffset = attribute_value(tvb, ndps_tree, foffset);  /
        break;
    case 5:                 /
        proto_tree_add_item(ndps_tree, hf_problem_type, tvb, foffset, 4, FALSE);
        foffset += 4;
        if (tvb_get_ntohl(tvb, foffset-4)==0) /
        {
            proto_tree_add_item(ndps_tree, hf_doc_access_problem_type, tvb, foffset, 4, FALSE);
            foffset = objectidentifier(tvb, ndps_tree, foffset);
        }
        else                /
        {
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Extended Error");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
        }
        foffset = objectidentification(tvb, ndps_tree, foffset);
        break;
    case 6:                 /
        number_of_items = tvb_get_ntohl(tvb, foffset); 
        proto_tree_add_uint(ndps_tree, hf_ndps_num_attributes, tvb, foffset, 4, number_of_items);
        foffset += 4;
        for (i = 1 ; i <= number_of_items; i++ )
        {
	    if (i > NDPS_MAX_ITEMS) {
	        proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	        break;
	    }
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Item %d", i);
            atree = proto_item_add_subtree(aitem, ett_ndps);
            proto_tree_add_item(atree, hf_problem_type, tvb, foffset, 4, FALSE);
            foffset += 4;
            if (tvb_get_ntohl(tvb, foffset-4)==0) /
            {
                proto_tree_add_item(atree, hf_attribute_problem_type, tvb, foffset, 4, FALSE);
                foffset += 4;
            }
            else                /
            {
                /
                bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Extended Error");
                btree = proto_item_add_subtree(bitem, ett_ndps);
                foffset = objectidentifier(tvb, btree, foffset);
                proto_item_set_end(bitem, tvb, foffset);
                /
            }
            foffset = attribute_value(tvb, atree, foffset);  /
            proto_item_set_end(aitem, tvb, foffset);
        }
        break;
    case 7:                 /
        proto_tree_add_item(ndps_tree, hf_problem_type, tvb, foffset, 4, FALSE);
        foffset += 4;
        if (tvb_get_ntohl(tvb, foffset-4)==0) /
        {
            proto_tree_add_item(ndps_tree, hf_update_problem_type, tvb, foffset, 4, FALSE);
            foffset += 4;
        }
        else                /
        {
            /
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Extended Error");
            atree = proto_item_add_subtree(aitem, ett_ndps);
            foffset = objectidentifier(tvb, atree, foffset);
            proto_item_set_end(aitem, tvb, foffset);
            /
        }
        foffset = objectidentification(tvb, ndps_tree, foffset);
        break;
    default:
        break;
    }
    return foffset;
}
