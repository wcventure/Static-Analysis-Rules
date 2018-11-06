static void
dissect_ospf_ls_ack(tvbuff_t *tvb, int offset, proto_tree *tree, guint8 version)
{
    /
    while (tvb_reported_length_remaining(tvb, offset) != 0) {
        if ( version == OSPF_VERSION_2)
	    offset = dissect_ospf_v2_lsa(tvb, offset, tree, FALSE);
        else
	    if ( version == OSPF_VERSION_3)
	      offset = dissect_ospf_v3_lsa(tvb, offset, tree, FALSE);
    }
}
