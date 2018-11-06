static void
dissect_ndps_ipx(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    proto_tree	    *ndps_tree = NULL;
    proto_item	    *ti;

    if (check_col(pinfo->cinfo, COL_PROTOCOL))
        col_set_str(pinfo->cinfo, COL_PROTOCOL, "NDPS");

    if (check_col(pinfo->cinfo, COL_INFO))
        col_clear(pinfo->cinfo, COL_INFO);
	
    if (tree) {
        ti = proto_tree_add_item(tree, proto_ndps, tvb, 0, -1, FALSE);
        ndps_tree = proto_item_add_subtree(ti, ett_ndps);
    }
    ndps_defrag(tvb, pinfo, ndps_tree);
}
