static int
server_entry(tvbuff_t* tvb, proto_tree *ndps_tree, int foffset)
{
    char        server_name[1024];
    guint32     number_of_items;
    guint32     i;
    guint32     data_type;
    proto_tree  *atree;
    proto_item  *aitem;
    proto_tree  *btree;
    proto_item  *bitem;

    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Server Info");
    atree = proto_item_add_subtree(aitem, ett_ndps);
    foffset = ndps_string(tvb, hf_ndps_server_name, ndps_tree, foffset, server_name, sizeof server_name);
    proto_item_append_text(aitem, ": %s", server_name);
    proto_tree_add_item(atree, hf_ndps_server_type, tvb, foffset, 4, FALSE);
    foffset += 4;
    foffset = print_address(tvb, atree, foffset);
    number_of_items = tvb_get_ntohl(tvb, foffset); 
    proto_tree_add_uint(atree, hf_ndps_num_servers, tvb, foffset, 4, number_of_items);
    foffset += 4;
    for (i = 1 ; i <= number_of_items; i++ )
    {
	if (i > NDPS_MAX_ITEMS) {
	    proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	    break;
	}
        bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Info %u", i);
        btree = proto_item_add_subtree(bitem, ett_ndps);
        data_type = tvb_get_ntohl(tvb, foffset);
        proto_tree_add_item(btree, hf_ndps_data_item_type, tvb, foffset, 4, FALSE);
        foffset += 4;
        switch (data_type) 
        {
        case 0:   /
            proto_tree_add_item(btree, hf_info_int, tvb, foffset, 1, FALSE);
            foffset++;
            break;
        case 1:   /
            proto_tree_add_item(btree, hf_info_int16, tvb, foffset, 2, FALSE);
            foffset += 2;
            break;
        case 2:   /
            proto_tree_add_item(btree, hf_info_int32, tvb, foffset, 4, FALSE);
            foffset += 4;
            break;
        case 3:   /
            proto_tree_add_item(btree, hf_info_boolean, tvb, foffset, 4, FALSE);
            foffset += 4;
            break;
        case 4:   /
        case 5:   /
            foffset = ndps_string(tvb, hf_info_string, btree, foffset, NULL, 0);
            break;
        default:
            break;
        }
        proto_item_set_end(bitem, tvb, foffset);
    }
    proto_item_set_end(aitem, tvb, foffset);
    return foffset;
}
