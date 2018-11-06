static int
commonarguments(tvbuff_t* tvb, proto_tree *ndps_tree, int foffset)
{
    guint32     number_of_items;
    guint32     i;
    proto_tree  *atree;
    proto_item  *aitem;
    proto_tree  *btree;
    proto_item  *bitem;

    aitem = proto_tree_add_text(ndps_tree, tvb, foffset, -1, "Common Arguments");
    atree = proto_item_add_subtree(aitem, ett_ndps);
    number_of_items = tvb_get_ntohl(tvb, foffset); 
    proto_tree_add_uint(atree, hf_ndps_num_args, tvb, foffset, 4, number_of_items);
    foffset += 4;
    for (i = 1 ; i <= number_of_items; i++ )
    {
	if (i > NDPS_MAX_ITEMS) {
	    proto_tree_add_text(atree, tvb, foffset, -1, "[Truncated]");
	    break;
	}
        bitem = proto_tree_add_text(atree, tvb, foffset, -1, "Argument %u", i);
        btree = proto_item_add_subtree(bitem, ett_ndps);
        foffset = attribute_value(tvb, atree, foffset);
        proto_item_set_end(bitem, tvb, foffset);
    }
    proto_item_set_end(aitem, tvb, foffset);
    return foffset;
}
