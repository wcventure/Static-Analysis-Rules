static void
dissect_ospf_db_desc(tvbuff_t *tvb, int offset, proto_tree *tree, 
		guint8 version, guint16 length)
{
    proto_tree *ospf_db_desc_tree=NULL;
    proto_item *ti;
    guint8 reserved;
    int orig_offset = offset;

    if (tree) {
	ti = proto_tree_add_text(tree, tvb, offset, length, "OSPF DB Description");
	ospf_db_desc_tree = proto_item_add_subtree(ti, ett_ospf_desc);

        switch (version ) {

	    case OSPF_VERSION_2:
                proto_tree_add_text(ospf_db_desc_tree, tvb, offset, 2, "Interface MTU: %u",
			    tvb_get_ntohs(tvb, offset));

		dissect_ospf_bitfield(ospf_db_desc_tree, tvb, offset + 2, &bfinfo_v2_options);
		dissect_ospf_bitfield(ospf_db_desc_tree, tvb, offset + 3, &bfinfo_dbd);

	        proto_tree_add_text(ospf_db_desc_tree, tvb, offset + 4, 4, "DD Sequence: %u",
			    tvb_get_ntohl(tvb, offset + 4));

                offset += 8;
                break;

            case OSPF_VERSION_3:

	        reserved = tvb_get_guint8(tvb, offset);
	        proto_tree_add_text(ospf_db_desc_tree, tvb, offset, 1, (reserved == 0 ? "Reserved: %u" : "Reserved: %u [incorrect, should be 0]"),
				reserved);

		dissect_ospf_bitfield(ospf_db_desc_tree, tvb, offset + 1, &bfinfo_v3_options);

                proto_tree_add_text(ospf_db_desc_tree, tvb, offset + 4, 2, "Interface MTU: %u",
			    tvb_get_ntohs(tvb, offset+4));

	        reserved = tvb_get_guint8(tvb, offset + 6);
	        proto_tree_add_text(ospf_db_desc_tree, tvb, offset + 6, 1, (reserved == 0 ? "Reserved: %u" : "Reserved: %u [incorrect, should be 0]"),
				reserved);

		dissect_ospf_bitfield(ospf_db_desc_tree, tvb, offset + 7, &bfinfo_dbd);

	        proto_tree_add_text(ospf_db_desc_tree, tvb, offset + 8, 4, "DD Sequence: %u",
			    tvb_get_ntohl(tvb, offset + 8));

                offset += 12;
                break;
	}
    }

    /
    /
    while (orig_offset + length > offset) {
      if ( version == OSPF_VERSION_2)
          offset = dissect_ospf_v2_lsa(tvb, offset, tree, FALSE);
      else
	  if ( version == OSPF_VERSION_3)
              offset = dissect_ospf_v3_lsa(tvb, offset, tree, FALSE);
    }

}
