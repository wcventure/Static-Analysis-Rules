static void
dissect_ospf_ls_upd(tvbuff_t *tvb, int offset, proto_tree *tree, guint8 version,
		guint16 length)
{
    proto_tree *ospf_lsa_upd_tree=NULL;
    proto_item *ti;
    guint32 lsa_nr;
    guint32 lsa_counter;

    ti = proto_tree_add_text(tree, tvb, offset, length, "LS Update Packet");
    ospf_lsa_upd_tree = proto_item_add_subtree(ti, ett_ospf_lsa_upd);

    lsa_nr = tvb_get_ntohl(tvb, offset);
    proto_tree_add_text(ospf_lsa_upd_tree, tvb, offset, 4, "Number of LSAs: %u",
    			lsa_nr);
    /
    offset += 4; /

    lsa_counter = 0;
    while (lsa_counter < lsa_nr) {
        if ( version == OSPF_VERSION_2)
	    offset = dissect_ospf_v2_lsa(tvb, offset, ospf_lsa_upd_tree, TRUE);
        else
            if ( version == OSPF_VERSION_3)
	        offset = dissect_ospf_v3_lsa(tvb, offset, ospf_lsa_upd_tree, TRUE);
        lsa_counter += 1;
    }
}
