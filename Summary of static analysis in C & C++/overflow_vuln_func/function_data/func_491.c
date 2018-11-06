static void
dissect_rsvp_session (proto_item *ti, proto_tree *rsvp_object_tree,
		      tvbuff_t *tvb,
		      int offset, int obj_length,
		      int class _U_, int type,
		      rsvp_conversation_info *rsvph)
{
    int offset2 = offset + 4;

    proto_item_set_text(ti, "%s", summary_session(tvb, offset));

    switch(type) {
    case RSVP_SESSION_TYPE_IPV4:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 1 - IPv4");
	proto_tree_add_item(rsvp_object_tree,
			    rsvp_filter[RSVPF_SESSION_IP],
			    tvb, offset2, 4, FALSE);

	proto_tree_add_item(rsvp_object_tree,
			    rsvp_filter[RSVPF_SESSION_PROTO], tvb,
			    offset2+4, 1, FALSE);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+5, 1,
			    "Flags: %x",
			    tvb_get_guint8(tvb, offset2+5));
	proto_tree_add_item(rsvp_object_tree,
			    rsvp_filter[RSVPF_SESSION_PORT], tvb,
			    offset2+6, 2, FALSE);

	/
	rsvph->session_type = RSVP_SESSION_TYPE_IPV4;
	SET_ADDRESS(&rsvph->destination, AT_IPv4, 4,
                    tvb_get_ptr(tvb, offset2, 4));
	rsvph->protocol = tvb_get_guint8(tvb, offset2+4);
	rsvph->udp_dest_port = tvb_get_ntohs(tvb, offset2+6);

	break;

    case RSVP_SESSION_TYPE_IPV6:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 2 - IPv6");
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, 16,
			    "Destination address: %s",
			    ip6_to_str((const struct e_in6_addr *)tvb_get_ptr(tvb, offset2, 16)));
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+16, 1,
			    "Protocol: %u",
			    tvb_get_guint8(tvb, offset2+16));
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+17, 1,
			    "Flags: %x",
			    tvb_get_guint8(tvb, offset2+17));
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+18, 2,
			    "Destination port: %u",
			    tvb_get_ntohs(tvb, offset2+18));
	/
	rsvph->session_type = RSVP_SESSION_TYPE_IPV6;

	break;

    case RSVP_SESSION_TYPE_IPV4_LSP:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 7 - IPv4 LSP");
	proto_tree_add_item(rsvp_object_tree,
			    rsvp_filter[RSVPF_SESSION_IP],
			    tvb, offset2, 4, FALSE);

	proto_tree_add_item(rsvp_object_tree,
			    rsvp_filter[RSVPF_SESSION_TUNNEL_ID],
			    tvb, offset2+6, 2, FALSE);

	proto_tree_add_text(rsvp_object_tree, tvb, offset2+8, 4,
			    "Extended Tunnel ID: %u (%s)",
			    tvb_get_ntohl(tvb, offset2+8),
			    ip_to_str(tvb_get_ptr(tvb, offset2+8, 4)));
	proto_tree_add_item_hidden(rsvp_object_tree,
				   rsvp_filter[RSVPF_SESSION_EXT_TUNNEL_ID],
				   tvb, offset2+8, 4, FALSE);

	/
	rsvph->session_type = RSVP_SESSION_TYPE_IPV4_LSP;
	SET_ADDRESS(&rsvph->destination, AT_IPv4, 4, 
		    tvb_get_ptr(tvb, offset2, 4));
	rsvph->udp_dest_port = tvb_get_ntohs(tvb, offset2+6);
	rsvph->ext_tunnel_id = tvb_get_ntohl(tvb, offset2 + 8);
	break;

    case RSVP_SESSION_TYPE_AGGREGATE_IPV4:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 9 - IPv4 Aggregate");
	proto_tree_add_item(rsvp_object_tree,
			    rsvp_filter[RSVPF_SESSION_IP],
			    tvb, offset2, 4, FALSE);

	proto_tree_add_text(rsvp_object_tree, tvb, offset2+7, 1,
			    "DSCP: %u (%s)",
			    tvb_get_guint8(tvb, offset2+7),
			    val_to_str(tvb_get_guint8(tvb, offset2+7),
				       dscp_vals, "Unknown (%d)"));
	/
	rsvph->session_type = RSVP_SESSION_TYPE_AGGREGATE_IPV4;
	SET_ADDRESS(&rsvph->destination, AT_IPv4, 4, 
		    tvb_get_ptr(tvb, offset2, 4));
	rsvph->dscp = tvb_get_guint8(tvb, offset2+7);
	rsvph->ext_tunnel_id = tvb_get_ntohl(tvb, offset2 + 8);
	break;

    case RSVP_SESSION_TYPE_IPV4_UNI:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 11 - IPv4 UNI");
	proto_tree_add_item(rsvp_object_tree,
			    rsvp_filter[RSVPF_SESSION_IP],
			    tvb, offset2, 4, FALSE);

	proto_tree_add_item(rsvp_object_tree,
			    rsvp_filter[RSVPF_SESSION_TUNNEL_ID],
			    tvb, offset2+6, 2, FALSE);

	proto_tree_add_text(rsvp_object_tree, tvb, offset2+8, 4,
			    "Extended IPv4 Address: %s",
			    ip_to_str(tvb_get_ptr(tvb, offset2+8, 4)));
	proto_tree_add_item_hidden(rsvp_object_tree,
				   rsvp_filter[RSVPF_SESSION_EXT_TUNNEL_ID],
				   tvb, offset2+8, 4, FALSE);

	/
	rsvph->session_type = RSVP_SESSION_TYPE_IPV4_UNI;
	SET_ADDRESS(&rsvph->destination, AT_IPv4, 4, 
		    tvb_get_ptr(tvb, offset2, 4));
	rsvph->udp_dest_port = tvb_get_ntohs(tvb, offset2+6);
	rsvph->ext_tunnel_id = tvb_get_ntohl(tvb, offset2 + 8);

	break;

    case RSVP_SESSION_TYPE_IPV4_E_NNI:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 15 - IPv4 E-NNI");
	proto_tree_add_item(rsvp_object_tree,
			    rsvp_filter[RSVPF_SESSION_IP],
			    tvb, offset2, 4, FALSE);

	proto_tree_add_item(rsvp_object_tree,
			    rsvp_filter[RSVPF_SESSION_TUNNEL_ID],
			    tvb, offset2+6, 2, FALSE);

	proto_tree_add_text(rsvp_object_tree, tvb, offset2+8, 4,
			    "Extended IPv4 Address: %s",
			    ip_to_str(tvb_get_ptr(tvb, offset2+8, 4)));
	proto_tree_add_item_hidden(rsvp_object_tree,
				   rsvp_filter[RSVPF_SESSION_EXT_TUNNEL_ID],
				   tvb, offset2+8, 4, FALSE);

	/
	rsvph->session_type = RSVP_SESSION_TYPE_IPV4_E_NNI;
	SET_ADDRESS(&rsvph->destination, AT_IPv4, 4, 
		    tvb_get_ptr(tvb, offset2, 4));
	rsvph->udp_dest_port = tvb_get_ntohs(tvb, offset2+6);
	rsvph->ext_tunnel_id = tvb_get_ntohl(tvb, offset2 + 8);

	break;

    default:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: Unknown (%u)",
			    type);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, obj_length-4,
			    "Data (%d bytes)", obj_length-4);
    }
}
