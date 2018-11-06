static int
cardinal_seq(tvbuff_t* tvb, packet_info* pinfo, proto_tree *ndps_tree, int foffset)
{
    guint32     number_of_items;
    guint32     length;
    guint32     ii;
    proto_tree  *atree;
    proto_item  *aitem, *expert_item;

    number_of_items = tvb_get_ntohl(tvb, foffset);
    expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
    foffset += 4;
    for (ii = 0; ii < number_of_items; ii++ )
    {
        if (ii >= NDPS_MAX_ITEMS) {
            expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
            break;
        }
        atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Cardinal %u", ii+1);
        length = tvb_get_ntohl(tvb, foffset);
        foffset += 4;
        if (length==4)
        {
            proto_tree_add_item(atree, hf_ndps_attribute_value, tvb, foffset, length, ENC_BIG_ENDIAN);
        }
        tvb_ensure_bytes_exist(tvb, foffset, length);
        foffset += length;
        foffset += (length%2);
        if ((int) foffset <= 0)
            THROW(ReportedBoundsError);
        proto_item_set_end(aitem, tvb, foffset);
    }
    return foffset;
}
