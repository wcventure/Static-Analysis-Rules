static gint
dissect_ssl2_record(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
                    guint32 offset, guint* conv_version,
                    gboolean *need_desegmentation,
                    SslDecryptSession* ssl)
{
    guint32 initial_offset;
    guint8  byte;
    guint8  record_length_length;
    guint32 record_length;
    gint    is_escape;
    gint16  padding_length;
    guint8  msg_type;
    const gchar *msg_type_str;
    guint32 available_bytes;
    proto_tree *ti;
    proto_tree *ssl_record_tree;

    initial_offset       = offset;
    byte                 = 0;
    record_length_length = 0;
    record_length        = 0;
    is_escape            = -1;
    padding_length       = -1;
    msg_type             = 0;
    msg_type_str         = NULL;
    available_bytes      = 0;
    ssl_record_tree      = NULL;

    /
    byte = tvb_get_guint8(tvb, offset);
    record_length_length = (byte & 0x80) ? 2 : 3;

    /
    available_bytes = tvb_length_remaining(tvb, offset);

    if (ssl_desegment && pinfo->can_desegment) {
        /
        if (available_bytes < record_length_length) {
            /
            pinfo->desegment_offset = offset;
            pinfo->desegment_len = record_length_length - available_bytes;
            *need_desegmentation = TRUE;
            return offset;
        }
    }

    /
    switch(record_length_length) {
    case 2:                     /
        record_length = (byte & 0x7f) << 8;
        byte = tvb_get_guint8(tvb, offset + 1);
        record_length += byte;
        break;
    case 3:                     /
        is_escape = (byte & 0x40) ? TRUE : FALSE;
        record_length = (byte & 0x3f) << 8;
        byte = tvb_get_guint8(tvb, offset + 1);
        record_length += byte;
        byte = tvb_get_guint8(tvb, offset + 2);
        padding_length = byte;
    }

    /
    if (ssl_desegment && pinfo->can_desegment) {
        /
        if (available_bytes < (record_length_length + record_length)) {
            /
            pinfo->desegment_offset = offset;
            pinfo->desegment_len = (record_length_length + record_length)
		                   - available_bytes;
            *need_desegmentation = TRUE;
            return offset;
        }
    }
    offset += record_length_length;

    /
    ti = proto_tree_add_item(tree, hf_ssl2_record, tvb, initial_offset,
                             record_length_length + record_length, 0);
    ssl_record_tree = proto_item_add_subtree(ti, ett_ssl_record);

    /
    msg_type = tvb_get_guint8(tvb, initial_offset + record_length_length);

    /
    if (*conv_version == SSL_VER_UNKNOWN)
    {
        if (ssl_looks_like_valid_pct_handshake(tvb,
                                               (initial_offset +
                                                record_length_length),
                                               record_length)) {
            *conv_version = SSL_VER_PCT;
            /
        }
        else if (msg_type >= 2 && msg_type <= 8)
        {
            *conv_version = SSL_VER_SSLv2;
            /
        }
    }

    /
    if (check_col(pinfo->cinfo, COL_PROTOCOL))
    {
        col_set_str(pinfo->cinfo, COL_PROTOCOL,
                    (*conv_version == SSL_VER_PCT) ? "PCT" : "SSLv2");
    }

    /
    msg_type_str = match_strval(msg_type,
                                (*conv_version == SSL_VER_PCT)
				? pct_msg_types : ssl_20_msg_types);
    if (!msg_type_str
        || ((*conv_version != SSL_VER_PCT) &&
	    !ssl_looks_like_valid_v2_handshake(tvb, initial_offset
					       + record_length_length,
					       record_length))
	|| ((*conv_version == SSL_VER_PCT) &&
	    !ssl_looks_like_valid_pct_handshake(tvb, initial_offset
						+ record_length_length,
						record_length)))
    {
        if (ssl_record_tree)
        {
            proto_item_set_text(ssl_record_tree, "%s Record Layer: %s",
                                (*conv_version == SSL_VER_PCT)
                                ? "PCT" : "SSLv2",
                                "Encrypted Data");

	    /
	    ti = proto_tree_add_uint(ssl_record_tree,
				     hf_ssl_record_version, tvb,
				     initial_offset, 0, 0x0002);
	    PROTO_ITEM_SET_GENERATED(ti);
        }

        if (check_col(pinfo->cinfo, COL_INFO))
            col_append_str(pinfo->cinfo, COL_INFO, "Encrypted Data");
        return initial_offset + record_length_length + record_length;
    }
    else
    {
        if (check_col(pinfo->cinfo, COL_INFO))
            col_append_str(pinfo->cinfo, COL_INFO, msg_type_str);

        if (ssl_record_tree)
        {
            proto_item_set_text(ssl_record_tree, "%s Record Layer: %s",
                                (*conv_version == SSL_VER_PCT)
                                ? "PCT" : "SSLv2",
                                msg_type_str);
        }
    }

    /
    if (ssl_record_tree)
    {
	/
	ti = proto_tree_add_uint(ssl_record_tree,
				 hf_ssl_record_version, tvb,
				 initial_offset, 0, 0x0002);
	PROTO_ITEM_SET_GENERATED(ti);

        /
        tvb_ensure_bytes_exist(tvb, offset, record_length_length);
        ti = proto_tree_add_uint (ssl_record_tree,
                                  hf_ssl_record_length, tvb,
                                  initial_offset, record_length_length,
                                  record_length);
    }
    if (ssl_record_tree && is_escape != -1)
    {
        proto_tree_add_boolean(ssl_record_tree,
                               hf_ssl2_record_is_escape, tvb,
                               initial_offset, 1, is_escape);
        }
    if (ssl_record_tree && padding_length != -1)
    {
        proto_tree_add_uint(ssl_record_tree,
                            hf_ssl2_record_padding_length, tvb,
                            initial_offset + 2, 1, padding_length);
    }

    /

    /
    offset = initial_offset + record_length_length;

    /
    if (ssl_record_tree)
    {
        proto_tree_add_item(ssl_record_tree,
                            (*conv_version == SSL_VER_PCT)
                            ? hf_pct_msg_type : hf_ssl2_msg_type,
                            tvb, offset, 1, 0);
    }
    offset++;                   /

    if (*conv_version != SSL_VER_PCT)
    {
        /
        switch (msg_type) {
        case SSL2_HND_CLIENT_HELLO:
            dissect_ssl2_hnd_client_hello(tvb, ssl_record_tree, offset, ssl);
            break;

        case SSL2_HND_CLIENT_MASTER_KEY:
            dissect_ssl2_hnd_client_master_key(tvb, ssl_record_tree, offset);
            break;

        case SSL2_HND_SERVER_HELLO:
            dissect_ssl2_hnd_server_hello(tvb, ssl_record_tree, offset, pinfo);
            break;

        case SSL2_HND_ERROR:
        case SSL2_HND_CLIENT_FINISHED:
        case SSL2_HND_SERVER_VERIFY:
        case SSL2_HND_SERVER_FINISHED:
        case SSL2_HND_REQUEST_CERTIFICATE:
        case SSL2_HND_CLIENT_CERTIFICATE:
            /
            break;

        default:                    /
            break;
        }
    }
    else
    {
        /
        switch (msg_type) {
        case PCT_MSG_CLIENT_HELLO:
			dissect_pct_msg_client_hello(tvb, ssl_record_tree, offset);
			break;
        case PCT_MSG_SERVER_HELLO:
			dissect_pct_msg_server_hello(tvb, ssl_record_tree, offset, pinfo);
			break;
        case PCT_MSG_CLIENT_MASTER_KEY:
			dissect_pct_msg_client_master_key(tvb, ssl_record_tree, offset);
			break;
        case PCT_MSG_SERVER_VERIFY:
			dissect_pct_msg_server_verify(tvb, ssl_record_tree, offset);
			break;
		case PCT_MSG_ERROR:
			dissect_pct_msg_error(tvb, ssl_record_tree, offset);
            break;

        default:                    /
            break;
        }
    }
    return (initial_offset + record_length_length + record_length);
}
