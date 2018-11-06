static void dissect_ipcp_compress_opt(const ip_tcp_opt *optp, tvbuff_t *tvb,
			int offset, guint length, packet_info *pinfo _U_,
			proto_tree *tree)
{
    guint8  ub;
    guint16 us;
    proto_item *tf;
    proto_tree *field_tree = NULL;

    tf = proto_tree_add_text(tree, tvb, offset, length, "%s: %u byte%s",
			     optp->name, length, plurality(length, "", "s"));

    field_tree = proto_item_add_subtree(tf, *optp->subtree_index);
    offset += 2;	/
    length -= 2;

    us = tvb_get_ntohs(tvb, offset);
    proto_tree_add_text( field_tree, tvb, offset, 2, "IP compression protocol: %s (0x%04x)",
			 val_to_str( us, ipcp_compress_proto_vals, "Unknown protocol" ),
			 us );
    offset += 2;	/
    length -= 2;

    if (length > 0) {
	switch ( us ) {
	case IPCP_COMPRESS_VJ_1172:
	case IPCP_COMPRESS_VJ:
	    /
	    ub = tvb_get_guint8( tvb, offset );
	    proto_tree_add_text( field_tree, tvb, offset, 1,
				 "Max slot id: %u (0x%02x)",
				 ub, ub );
	    offset++;
	    length--;

	    if ( length > 0 ) {
		/
		ub = tvb_get_guint8( tvb, offset );
		proto_tree_add_text( field_tree, tvb, offset, 1,
				     "Compress slot id: %s (0x%02x)",
				     ub ? "yes" : "no",  ub );
		offset++;
		length--;
	    }
	    break;

	    
	case IPCP_COMPRESS_IPHC:
	    if ( length < 2 ) {
		break;
	    }
	    us = tvb_get_ntohs(tvb, offset);
	    proto_tree_add_text( field_tree, tvb, offset, 2,
				 "TCP space: %u (0x%04x)",
				 us, us );
	    offset += 2;
	    length -= 2;


	    if ( length < 2 ) {
		break;
	    }
	    us = tvb_get_ntohs(tvb, offset);
	    proto_tree_add_text( field_tree, tvb, offset, 2,
				 "Non-TCP space: %u (0x%04x)",
				 us, us );
	    offset += 2;
	    length -= 2;


	    if ( length < 2 ) {
		break;
	    }
	    us = tvb_get_ntohs(tvb, offset);
	    proto_tree_add_text( field_tree, tvb, offset, 2,
				 "Max period: %u (0x%04x) compressed packets",
				 us, us );
	    offset += 2;
	    length -= 2;


	    if ( length < 2 ) {
		break;
	    }
	    us = tvb_get_ntohs(tvb, offset);
	    proto_tree_add_text( field_tree, tvb, offset, 2,
				 "Max time: %u (0x%04x) seconds",
				 us, us );
	    offset += 2;
	    length -= 2;


	    if ( length < 2 ) {
		break;
	    }
	    us = tvb_get_ntohs(tvb, offset);
	    proto_tree_add_text( field_tree, tvb, offset, 2,
				 "Max header: %u (0x%04x) bytes",
				 us, us );
	    offset += 2;
	    length -= 2;

	    if ( length > 0 ) {
		/
		tf = proto_tree_add_text(field_tree, tvb, offset, length,
					 "Suboptions: (%u byte%s)",
					 length, plurality(length, "", "s"));
		field_tree = proto_item_add_subtree(tf, *optp->subtree_index);
		dissect_ip_tcp_options(tvb, offset, length,
				       ipcp_iphc_subopts, N_IPCP_IPHC_SUBOPTS, -1,
				       pinfo, field_tree);
	    }
	    return;
	}

	if (length > 0) {
	    proto_tree_add_text(field_tree, tvb, offset, length,
				"Data (%d byte%s)", length,
				plurality(length, "", "s"));
	}
    }
}
