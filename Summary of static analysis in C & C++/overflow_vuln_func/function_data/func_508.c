static void
dissect_rsvp_msg_tree(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
		      int tree_mode, rsvp_conversation_info *rsvph)
{
    proto_tree *rsvp_tree = NULL;
    proto_tree *rsvp_header_tree;
    proto_tree *rsvp_object_tree;
    proto_tree *ti;
    guint16 cksum, computed_cksum;
    vec_t cksum_vec[1];
    int offset = 0;
    int len;
    guint8 ver_flags;
    guint8 message_type;
    int session_off, tempfilt_off;
    int msg_length;
    int obj_length;
    int offset2;

    offset = 0;
    len = 0;
    ver_flags = tvb_get_guint8(tvb, 0);
    msg_length = tvb_get_ntohs(tvb, 6);
    message_type = tvb_get_guint8(tvb, 1);

    ti = proto_tree_add_item(tree, proto_rsvp, tvb, offset, msg_length,
			     FALSE);
    rsvp_tree = proto_item_add_subtree(ti, tree_mode);
    if (pinfo->ipproto == IP_PROTO_RSVPE2EI) 
	proto_item_append_text(rsvp_tree, " (E2E-IGNORE)");
    proto_item_append_text(rsvp_tree, ": ");
    proto_item_append_text(rsvp_tree, val_to_str(message_type, message_type_vals,
						 "Unknown (%u). "));
    find_rsvp_session_tempfilt(tvb, 0, &session_off, &tempfilt_off);
    if (session_off)
	proto_item_append_text(rsvp_tree, summary_session(tvb, session_off));
    if (tempfilt_off)
	proto_item_append_text(rsvp_tree, summary_template(tvb, tempfilt_off));

    ti = proto_tree_add_text(rsvp_tree, tvb, offset, 8, "RSVP Header. %s",
			     val_to_str(message_type, message_type_vals,
					"Unknown Message (%u). "));
    if (pinfo->ipproto == IP_PROTO_RSVPE2EI) 
	proto_item_append_text(ti, " (E2E-IGNORE)");
    rsvp_header_tree = proto_item_add_subtree(ti, TREE(TT_HDR));

    proto_tree_add_text(rsvp_header_tree, tvb, offset, 1, "RSVP Version: %u",
			(ver_flags & 0xf0)>>4);
    proto_tree_add_text(rsvp_header_tree, tvb, offset, 1, "Flags: %02x",
			ver_flags & 0xf);
    proto_tree_add_uint(rsvp_header_tree, rsvp_filter[RSVPF_MSG], tvb,
			offset+1, 1, message_type);
    switch (RSVPF_MSG + message_type) {

    case RSVPF_PATH:
    case RSVPF_RESV:
    case RSVPF_PATHERR:
    case RSVPF_RESVERR:
    case RSVPF_PATHTEAR:
    case RSVPF_RESVTEAR:
    case RSVPF_RCONFIRM:
    case RSVPF_RTEARCONFIRM:
    case RSVPF_BUNDLE:
    case RSVPF_ACK:
    case RSVPF_SREFRESH:
    case RSVPF_HELLO:
	case RSVPF_NOTIFY:
	proto_tree_add_boolean_hidden(rsvp_header_tree, rsvp_filter[RSVPF_MSG + message_type], tvb,
				      offset+1, 1, 1);
	break;

    default:
	proto_tree_add_protocol_format(rsvp_header_tree, proto_malformed, tvb, offset+1, 1,
				       "Invalid message type: %u", message_type);
	return;
    }

    cksum = tvb_get_ntohs(tvb, offset+2);
    if (!pinfo->fragmented && (int) tvb_length(tvb) >= msg_length) {
	/
	cksum_vec[0].ptr = tvb_get_ptr(tvb, 0, msg_length);
	cksum_vec[0].len = msg_length;
	computed_cksum = in_cksum(&cksum_vec[0], 1);
	if (computed_cksum == 0) {
	    proto_tree_add_text(rsvp_header_tree, tvb, offset+2, 2,
				"Message Checksum: 0x%04x [correct]",
				cksum);
	} else {
	    proto_tree_add_text(rsvp_header_tree, tvb, offset+2, 2,
				"Message Checksum: 0x%04x [incorrect, should be 0x%04x]",
				cksum,
				in_cksum_shouldbe(cksum, computed_cksum));
	}
    } else {
	proto_tree_add_text(rsvp_header_tree, tvb, offset+2, 2,
			    "Message Checksum: 0x%04x",
			    cksum);
    }
    proto_tree_add_text(rsvp_header_tree, tvb, offset+4, 1,
			"Sending TTL: %u",
			tvb_get_guint8(tvb, offset+4));
    proto_tree_add_text(rsvp_header_tree, tvb, offset+6, 2,
			"Message length: %u", msg_length);

    offset = 8;
    len = 8;

    if (message_type == RSVP_MSG_BUNDLE) {
	/
	if (rsvp_bundle_dissect) {
	    int len = 8;
	    while (len < msg_length) {
		gint sub_len;
		tvbuff_t *tvb_sub;
		sub_len = tvb_get_ntohs(tvb, len+6);
		tvb_sub = tvb_new_subset(tvb, len, sub_len, sub_len);
		dissect_rsvp_msg_tree(tvb_sub, pinfo, rsvp_tree, TREE(TT_BUNDLE_COMPMSG), rsvph);
		len += sub_len;
	    }
	} else {
	    proto_tree_add_text(rsvp_tree, tvb, offset, msg_length - len,
				"Bundle Component Messages Not Dissected");
	}
	return;
    }

    while (len < msg_length) {
	guint8 class;
	guint8 type;

	obj_length = tvb_get_ntohs(tvb, offset);
	class = tvb_get_guint8(tvb, offset+2);
	type = tvb_get_guint8(tvb, offset+3);
	ti = proto_tree_add_item(rsvp_tree, rsvp_filter[rsvp_class_to_filter_num(class)],
				 tvb, offset, obj_length, FALSE);
	rsvp_object_tree = proto_item_add_subtree(ti, TREE(rsvp_class_to_tree_type(class)));
	if (obj_length < 4) {
	    proto_tree_add_text(rsvp_object_tree, tvb, offset, 2,
				"Length: %u (bogus, must be >= 4)", obj_length);
	    break;
	}
	proto_tree_add_text(rsvp_object_tree, tvb, offset, 2,
			    "Length: %u", obj_length);
	proto_tree_add_uint(rsvp_object_tree, rsvp_filter[RSVPF_OBJECT], tvb,
			    offset+2, 1, class);

	offset2 = offset+4;

	switch(class) {

	case RSVP_CLASS_SESSION:
	    dissect_rsvp_session(ti, rsvp_object_tree, tvb, offset, obj_length, class, type, rsvph);
	    break;

	case RSVP_CLASS_HOP:
	    dissect_rsvp_hop(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_TIME_VALUES:
	    dissect_rsvp_time_values(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_ERROR:
	    dissect_rsvp_error(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_SCOPE:
	    dissect_rsvp_scope(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_STYLE:
	    dissect_rsvp_style(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_CONFIRM:
	    dissect_rsvp_confirm(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_SENDER_TEMPLATE:
	case RSVP_CLASS_FILTER_SPEC:
	    dissect_rsvp_template_filter(ti, rsvp_object_tree, tvb, offset, obj_length, class, type, rsvph);
	    break;

	case RSVP_CLASS_SENDER_TSPEC:
	    dissect_rsvp_tspec(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_FLOWSPEC:
	    dissect_rsvp_flowspec(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_ADSPEC:
	    dissect_rsvp_adspec(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_INTEGRITY:
	    dissect_rsvp_integrity(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_POLICY:
	    dissect_rsvp_policy(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_LABEL_REQUEST:
	    dissect_rsvp_label_request(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_UPSTREAM_LABEL:
	case RSVP_CLASS_SUGGESTED_LABEL:
	case RSVP_CLASS_LABEL:
	    dissect_rsvp_label(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_LABEL_SET:
	    dissect_rsvp_label_set(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_SESSION_ATTRIBUTE:
	    dissect_rsvp_session_attribute(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_EXPLICIT_ROUTE:
	    dissect_rsvp_explicit_route(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_RECORD_ROUTE:
	    dissect_rsvp_record_route(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_MESSAGE_ID:
	    dissect_rsvp_message_id(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_MESSAGE_ID_ACK:
	    dissect_rsvp_message_id_ack(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_MESSAGE_ID_LIST:
	    dissect_rsvp_message_id_list(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_HELLO:
	    dissect_rsvp_hello(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_DCLASS:
	    dissect_rsvp_dclass(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_ADMIN_STATUS:
	    dissect_rsvp_admin_status(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_ASSOCIATION:
	    dissect_rsvp_association(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_LSP_TUNNEL_IF_ID:
	    dissect_rsvp_lsp_tunnel_if_id(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_GENERALIZED_UNI:
	    dissect_rsvp_gen_uni(ti, rsvp_object_tree, tvb, offset, obj_length, class, type, rsvph);
	    break;

	case RSVP_CLASS_CALL_ID:
	    dissect_rsvp_call_id(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_RESTART_CAP:
	    dissect_rsvp_restart_cap(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_PROTECTION:
	    dissect_rsvp_protection_info(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_FAST_REROUTE:
	    dissect_rsvp_fast_reroute(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_DETOUR:
	    dissect_rsvp_detour(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_DIFFSERV:
	    dissect_rsvp_diffserv(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_CLASSTYPE:
	    dissect_rsvp_diffserv_aware_te(ti, rsvp_object_tree, tvb, offset, obj_length, class, type);
	    break;

	case RSVP_CLASS_NULL:
	default:
	    proto_tree_add_text(rsvp_object_tree, tvb, offset2, obj_length - 4,
				"Data (%d bytes)", obj_length - 4);
	    break;
	}

	offset += obj_length;
	len += obj_length;
    }
}
