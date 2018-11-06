static int
credentials(tvbuff_t* tvb, proto_tree *ndps_tree, int foffset)
{
    guint32     cred_type=0;
    guint32     length=0;
    guint32     number_of_items;
    guint32     i;
    proto_tree  *atree;
    proto_item  *aitem;

    cred_type = tvb_get_ntohl(tvb, foffset);
    proto_tree_add_item(ndps_tree, hf_ndps_cred_type, tvb, foffset, 4, FALSE);
    foffset += 4;
    switch (cred_type)
    {
    case 0:
        foffset = ndps_string(tvb, hf_ndps_user_name, ndps_tree, foffset, NULL, 0);
        number_of_items=tvb_get_ntohl(tvb, foffset);
        proto_tree_add_uint(ndps_tree, hf_ndps_num_passwords, tvb, foffset, 4, number_of_items);
        foffset += 4;
        for (i = 1 ; i <= number_of_items; i++ )
        {
	    if (i > NDPS_MAX_ITEMS) {
	        proto_tree_add_text(ndps_tree, tvb, foffset, -1, "[Truncated]");
	        break;
	    }
            aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Password %d", i);
            atree = proto_item_add_subtree(aitem, ett_ndps);
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length!=0)
            {
                tvb_ensure_bytes_exist(tvb, foffset, length);
                proto_tree_add_item(atree, hf_ndps_password, tvb, foffset, length, FALSE);
            }
            proto_item_set_end(aitem, tvb, foffset);
            foffset += length;
        }
        break;
    case 1:
        length = tvb_get_ntohl(tvb, foffset);
        foffset += 4;
        if (length!=0)
        {
            tvb_ensure_bytes_exist(tvb, foffset, length);
            proto_tree_add_item(ndps_tree, hf_ndps_certified, tvb, foffset, length, FALSE);
        }
        foffset += length;
        break;
    case 2:
        foffset = ndps_string(tvb, hf_ndps_server_name, ndps_tree, foffset, NULL, 0);
        foffset += 2;
        proto_tree_add_item(ndps_tree, hf_ndps_connection, tvb, foffset, 2, FALSE);
        foffset += 2;
        break;
    case 3:
        length=tvb_get_ntohl(tvb, foffset);
        foffset = ndps_string(tvb, hf_ndps_server_name, ndps_tree, foffset, NULL, 0);
        if (length == 0) 
        {
            foffset += 2;
        }
        if (tvb_get_ntohs(tvb, foffset)==0)  /
        {
            foffset+=2;
            if (tvb_get_ntohs(tvb, foffset)==0)  /
            {
                foffset += 2;
            }
        }
        proto_tree_add_item(ndps_tree, hf_ndps_connection, tvb, foffset, 2, FALSE);
        foffset += 2;
        foffset = ndps_string(tvb, hf_ndps_user_name, ndps_tree, foffset, NULL, 0);
        break;
    case 4:
        foffset = ndps_string(tvb, hf_ndps_server_name, ndps_tree, foffset, NULL, 0);
        foffset += 2;
        proto_tree_add_item(ndps_tree, hf_ndps_connection, tvb, foffset, 2, FALSE);
        foffset += 2;
        foffset = ndps_string(tvb, hf_ndps_user_name, ndps_tree, foffset, NULL, 0);
        foffset += 8;   /
        proto_tree_add_item(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, FALSE);
        foffset += 4;	/
        foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL, 0);
        foffset = ndps_string(tvb, hf_ndps_tree, ndps_tree, foffset, NULL, 0);
        break;
    default:
        break;
    }
    return foffset;
}
