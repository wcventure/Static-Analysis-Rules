static gint 
rsvp_equal (gconstpointer k1, gconstpointer k2)
{
    const struct rsvp_request_key *key1 = (const struct rsvp_request_key*) k1;
    const struct rsvp_request_key *key2 = (const struct rsvp_request_key*) k2;

    if (key1->conversation != key2->conversation) {
	return 0;
    }

    if (key1->session_type != key2->session_type) {
	return 0;
    }

    switch (key1->session_type) {
    case RSVP_SESSION_TYPE_IPV4:
	if (ADDRESSES_EQUAL(&key1->u.session_ipv4.destination,
			    &key2->u.session_ipv4.destination) == FALSE)
	    return 0;

	if (key1->u.session_ipv4.protocol != key2->u.session_ipv4.protocol)
	    return 0;
	
	if (key1->u.session_ipv4.udp_dest_port != key2->u.session_ipv4.udp_dest_port)
	    return 0;
	
	break;

    case RSVP_SESSION_TYPE_IPV6:
	/
	break;

    case RSVP_SESSION_TYPE_IPV4_LSP:
	if (ADDRESSES_EQUAL(&key1->u.session_ipv4_lsp.destination,
			    &key2->u.session_ipv4_lsp.destination) == FALSE)
	    return 0;
	
	if (key1->u.session_ipv4_lsp.udp_dest_port != 
	    key2->u.session_ipv4_lsp.udp_dest_port)
	    return 0;

	
	if (key1->u.session_ipv4_lsp.ext_tunnel_id != 
	    key2->u.session_ipv4_lsp.ext_tunnel_id)
	    return 0;

	break;

    case RSVP_SESSION_TYPE_AGGREGATE_IPV4:
	if (ADDRESSES_EQUAL(&key1->u.session_agg_ipv4.destination,
			    &key2->u.session_agg_ipv4.destination) == FALSE)
	    return 0;

	if (key1->u.session_agg_ipv4.dscp != key2->u.session_agg_ipv4.dscp)
	    return 0;
	
	break;

    case RSVP_SESSION_TYPE_AGGREGATE_IPV6:
	/
	break;

    case RSVP_SESSION_TYPE_IPV4_UNI:
	if (ADDRESSES_EQUAL(&key1->u.session_ipv4_uni.destination,
			    &key2->u.session_ipv4_uni.destination) == FALSE)
	    return 0;
	
	if (key1->u.session_ipv4_uni.udp_dest_port != 
	    key2->u.session_ipv4_uni.udp_dest_port)
	    return 0;

	
	if (key1->u.session_ipv4_uni.ext_tunnel_id != 
	    key2->u.session_ipv4_uni.ext_tunnel_id)
	    return 0;

	break;

    case RSVP_SESSION_TYPE_IPV4_E_NNI:
	if (ADDRESSES_EQUAL(&key1->u.session_ipv4_enni.destination,
			    &key2->u.session_ipv4_enni.destination) == FALSE)
	    return 0;
	
	if (key1->u.session_ipv4_enni.udp_dest_port != 
	    key2->u.session_ipv4_enni.udp_dest_port)
	    return 0;

	
	if (key1->u.session_ipv4_enni.ext_tunnel_id != 
	    key2->u.session_ipv4_enni.ext_tunnel_id)
	    return 0;

	break;

    default:
	/
	break;
    }

    if (ADDRESSES_EQUAL(&key1->source_info.source, 
			&key2->source_info.source) == FALSE)
	return 0;

    if (key1->source_info.udp_source_port != key2->source_info.udp_source_port)
	return 0;

    /
    return 1;
}
