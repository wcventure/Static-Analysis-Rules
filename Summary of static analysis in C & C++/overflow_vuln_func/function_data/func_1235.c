static void
dissect_ospf_ls_req(tvbuff_t *tvb, int offset, proto_tree *tree, guint8 version)
{
    proto_tree *ospf_lsr_tree;
    proto_item *ti;
    guint32 ls_type;
    guint16 reserved;

    /
    /
    while (tvb_reported_length_remaining(tvb, offset) != 0) {
	ti = proto_tree_add_text(tree, tvb, offset, OSPF_LS_REQ_LENGTH,
				 "Link State Request");
	ospf_lsr_tree = proto_item_add_subtree(ti, ett_ospf_lsr);

        switch ( version ) {

    	    case OSPF_VERSION_2:
 	        ls_type = tvb_get_ntohl(tvb, offset);
  	        proto_tree_add_item(ospf_lsr_tree, ospf_filter[OSPFF_LS_TYPE],
				    tvb, offset, 4, FALSE);
	        break;
    	    case OSPF_VERSION_3:
 	        reserved = tvb_get_ntohs(tvb, offset);
 	        proto_tree_add_text(ospf_lsr_tree, tvb, offset, 2,
 	            (reserved == 0 ? "Reserved: %u" :  "Reserved: %u [incorrect, should be 0]"), reserved);
 	        ls_type = tvb_get_ntohs(tvb, offset+2);
	        proto_tree_add_text(ospf_lsr_tree, tvb, offset+2, 2, "LS Type: %s (0x%04x)",
			    val_to_str(ls_type, v3_ls_type_vals, "Unknown"),
			    ls_type);
		break;
        }


	proto_tree_add_text(ospf_lsr_tree, tvb, offset + 4, 4, "Link State ID: %s",
			    ip_to_str(tvb_get_ptr(tvb, offset + 4, 4)));
	proto_tree_add_item(ospf_lsr_tree, ospf_filter[OSPFF_ADV_ROUTER],
			    tvb, offset + 8, 4, FALSE);

	offset += 12;
    }
}
