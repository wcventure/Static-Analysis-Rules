static void
dissect_ssl3_heartbeat(tvbuff_t *tvb, packet_info *pinfo,
                       proto_tree *tree, guint32 offset,
                       const SslSession *session, guint32 record_length,
                       gboolean decrypted)
{
    /

    proto_item  *ti;
    proto_tree  *tls_heartbeat_tree;
    const gchar *type;
    guint8       byte;
    guint16      payload_length;
    guint16      padding_length;

    tls_heartbeat_tree = NULL;

    if (tree) {
        ti = proto_tree_add_item(tree, hf_ssl_heartbeat_message, tvb,
                                 offset, record_length - 32, ENC_NA);
        tls_heartbeat_tree = proto_item_add_subtree(ti, ett_ssl_heartbeat);
    }

    /

    /
    byte = tvb_get_guint8(tvb, offset);
    type = try_val_to_str(byte, tls_heartbeat_type);

    payload_length = tvb_get_ntohs(tvb, offset + 1);
    padding_length = record_length - 3 - payload_length;

    /
    if (type && ((payload_length <= record_length - 16 - 3) || decrypted)) {
        col_append_fstr(pinfo->cinfo, COL_INFO, "Heartbeat %s", type);
    } else {
        col_append_str(pinfo->cinfo, COL_INFO, "Encrypted Heartbeat");
    }

    if (tree) {
        if (type && ((payload_length <= record_length - 16 - 3) || decrypted)) {
            proto_item_set_text(tree, "%s Record Layer: Heartbeat "
                                "%s",
                                val_to_str_const(session->version, ssl_version_short_names, "SSL"),
                                type);
            proto_tree_add_item(tls_heartbeat_tree, hf_ssl_heartbeat_message_type,
                                tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            ti = proto_tree_add_uint(tls_heartbeat_tree, hf_ssl_heartbeat_message_payload_length,
                                     tvb, offset, 2, payload_length);
            offset += 2;
            if (payload_length > record_length - 16 - 3) {
                expert_add_info_format(pinfo, ti, &ei_ssl3_heartbeat_payload_length,
                                       "Invalid payload heartbeat length (%d)", payload_length);
                /
                payload_length = record_length - 16 - 3;
                padding_length = 16;
                proto_item_append_text (ti, " (invalid, using %u to decode payload)", payload_length);
            }
            proto_tree_add_bytes_format(tls_heartbeat_tree, hf_ssl_heartbeat_message_payload,
                                        tvb, offset, payload_length,
                                        NULL, "Payload (%u byte%s)",
                                        payload_length,
                                        plurality(payload_length, "", "s"));
            offset += payload_length;
            proto_tree_add_bytes_format(tls_heartbeat_tree, hf_ssl_heartbeat_message_padding,
                                        tvb, offset, padding_length,
                                        NULL, "Padding and HMAC (%u byte%s)",
                                        padding_length,
                                        plurality(padding_length, "", "s"));
        } else {
            proto_item_set_text(tree,
                                "%s Record Layer: Encrypted Heartbeat",
                                val_to_str_const(session->version, ssl_version_short_names, "SSL"));
            proto_item_set_text(tls_heartbeat_tree,
                                "Encrypted Heartbeat Message");
        }
    }
}
