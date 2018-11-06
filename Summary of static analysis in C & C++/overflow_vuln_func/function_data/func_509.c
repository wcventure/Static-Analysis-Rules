static void
dissect_rsvp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    guint8 ver_flags;
    guint8 message_type;
    int msg_length;
    int session_off, tempfilt_off;
    rsvp_conversation_info *rsvph;


    conversation_t *conversation;
    struct rsvp_request_key request_key, *new_request_key;
    struct rsvp_request_val *request_val = NULL;

    if (check_col(pinfo->cinfo, COL_PROTOCOL))
	col_set_str(pinfo->cinfo, COL_PROTOCOL, 
		    (pinfo->ipproto == IP_PROTO_RSVPE2EI) ? "RSVP-E2EI" : "RSVP");
    if (check_col(pinfo->cinfo, COL_INFO))
        col_clear(pinfo->cinfo, COL_INFO);

    ver_flags = tvb_get_guint8(tvb, 0);
    message_type = tvb_get_guint8(tvb, 1);
    msg_length = tvb_get_ntohs(tvb, 6);

    rsvph = ep_alloc(sizeof(rsvp_conversation_info));

    /
    SET_ADDRESS(&rsvph->source, pinfo->src.type, pinfo->src.len, pinfo->src.data);
    SET_ADDRESS(&rsvph->destination, pinfo->dst.type, pinfo->dst.len, pinfo->dst.data);

    if (check_col(pinfo->cinfo, COL_INFO)) {
        col_add_str(pinfo->cinfo, COL_INFO,
            val_to_str(message_type, message_type_vals, "Unknown (%u). "));
	find_rsvp_session_tempfilt(tvb, 0, &session_off, &tempfilt_off);
	if (session_off)
	    col_append_str(pinfo->cinfo, COL_INFO, summary_session(tvb, session_off));
	if (tempfilt_off)
	    col_append_str(pinfo->cinfo, COL_INFO, summary_template(tvb, tempfilt_off));
    }

    if (check_col(pinfo->cinfo, COL_INFO)) {
	col_add_str(pinfo->cinfo, COL_INFO,
		    val_to_str(message_type, message_type_vals, "Unknown (%u). "));
	if (message_type == RSVP_MSG_BUNDLE) {
	    col_set_str(pinfo->cinfo, COL_INFO,
			rsvp_bundle_dissect ?
			"Component Messages Dissected" :
			"Component Messages Not Dissected");
	} else {
	    find_rsvp_session_tempfilt(tvb, 0, &session_off, &tempfilt_off);
	    if (session_off)
		col_append_str(pinfo->cinfo, COL_INFO, summary_session(tvb, session_off));
	    if (tempfilt_off)
		col_append_str(pinfo->cinfo, COL_INFO, summary_template(tvb, tempfilt_off));
	}
    }

    if (tree) {
	dissect_rsvp_msg_tree(tvb, pinfo, tree, TREE(TT_RSVP), rsvph);
    }

    /
    conversation = find_conversation(pinfo->fd->num, &pinfo->src, &pinfo->dst,
				     pinfo->ptype, pinfo->srcport, 
				     pinfo->destport, 0);

    if (conversation == NULL) {
	/
	conversation = 
	    conversation_new(pinfo->fd->num, &pinfo->src, &pinfo->dst, 
			     pinfo->ptype, pinfo->srcport, 
			     pinfo->destport, 0);
    }

    /
    request_key.conversation = conversation->index;
    request_key.session_type = rsvph->session_type;

    switch (request_key.session_type) {
    case RSVP_SESSION_TYPE_IPV4:
	SET_ADDRESS(&request_key.u.session_ipv4.destination, 
		    rsvph->destination.type, rsvph->destination.len,
		    rsvph->destination.data);
	request_key.u.session_ipv4.protocol = rsvph->protocol;
	request_key.u.session_ipv4.udp_dest_port = rsvph->udp_dest_port;
	break;

    case RSVP_SESSION_TYPE_IPV6:
	/
	break;

    case RSVP_SESSION_TYPE_IPV4_LSP:
	SET_ADDRESS(&request_key.u.session_ipv4_lsp.destination, 
		    rsvph->destination.type, rsvph->destination.len,
		    rsvph->destination.data);
	request_key.u.session_ipv4_lsp.udp_dest_port = rsvph->udp_dest_port;
	request_key.u.session_ipv4_lsp.ext_tunnel_id = rsvph->ext_tunnel_id;
	break;

    case RSVP_SESSION_TYPE_AGGREGATE_IPV4:
	SET_ADDRESS(&request_key.u.session_agg_ipv4.destination, 
		    rsvph->destination.type, rsvph->destination.len,
		    rsvph->destination.data);
	request_key.u.session_agg_ipv4.dscp = rsvph->dscp;
	break;

    case RSVP_SESSION_TYPE_IPV4_UNI:
	SET_ADDRESS(&request_key.u.session_ipv4_uni.destination, 
		    rsvph->destination.type, rsvph->destination.len,
		    rsvph->destination.data);
	request_key.u.session_ipv4_uni.udp_dest_port = rsvph->udp_dest_port;
	request_key.u.session_ipv4_uni.ext_tunnel_id = rsvph->ext_tunnel_id;
	break;

    case RSVP_SESSION_TYPE_IPV4_E_NNI:
	SET_ADDRESS(&request_key.u.session_ipv4_enni.destination, 
		    rsvph->destination.type, rsvph->destination.len,
		    rsvph->destination.data);
	request_key.u.session_ipv4_enni.udp_dest_port = rsvph->udp_dest_port;
	request_key.u.session_ipv4_enni.ext_tunnel_id = rsvph->ext_tunnel_id;
	break;
    default:
	/
	break;
    }

    SET_ADDRESS(&request_key.source_info.source, 
		rsvph->source.type, rsvph->source.len, rsvph->source.data);
    request_key.source_info.udp_source_port = rsvph->udp_source_port;

    /
    request_val = 
	(struct rsvp_request_val *) g_hash_table_lookup(rsvp_request_hash,
							&request_key);

    /
    if (!request_val) {
	new_request_key = se_alloc(sizeof(struct rsvp_request_key));
	*new_request_key = request_key;

	request_val = se_alloc(sizeof(struct rsvp_request_val));
	request_val->value = conversation->index;

	g_hash_table_insert(rsvp_request_hash, new_request_key, request_val);
    }

    tap_queue_packet(rsvp_tap, pinfo, rsvph);
}
