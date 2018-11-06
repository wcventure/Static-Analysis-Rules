void ssl_set_master_secret(guint32 frame_num, address *addr_srv, address *addr_cli,
                           port_type ptype, guint32 port_srv, guint32 port_cli,
                           guint32 version, gint cipher, const guchar *_master_secret,
                           const guchar *_client_random, const guchar *_server_random,
                           guint32 client_seq, guint32 server_seq)
{
    conversation_t    *conversation;
    SslDecryptSession *ssl;
    guint              iv_len;

    ssl_debug_printf("\nssl_set_master_secret enter frame #%u\n", frame_num);

    conversation = find_conversation(frame_num, addr_srv, addr_cli, ptype, port_srv, port_cli, 0);

    if (!conversation) {
        /
        conversation = conversation_new(frame_num, addr_srv, addr_cli, ptype, port_srv, port_cli, 0);
        ssl_debug_printf("  new conversation = %p created\n", (void *)conversation);
    }
    ssl = ssl_get_session(conversation, ssl_handle);

    ssl_debug_printf("  conversation = %p, ssl_session = %p\n", (void *)conversation, (void *)ssl);

    ssl_set_server(&ssl->session, addr_srv, ptype, port_srv);

    /
    if ((ssl->session.version==SSL_VER_UNKNOWN) && (version!=SSL_VER_UNKNOWN)) {
        switch (version) {
        case SSLV3_VERSION:
        case TLSV1_VERSION:
        case TLSV1DOT1_VERSION:
        case TLSV1DOT2_VERSION:
            ssl->session.version = version;
            ssl->state |= SSL_VERSION;
            ssl_debug_printf("%s set version 0x%04X -> state 0x%02X\n", G_STRFUNC, ssl->session.version, ssl->state);
            break;
        default:
            /
            ssl_debug_printf("%s WARNING must pass ProtocolVersion, not 0x%04x!\n", G_STRFUNC, version);
            break;
        }
    }

    /
    if (cipher > 0) {
        ssl->session.cipher = cipher;
        if (!(ssl->cipher_suite = ssl_find_cipher(ssl->session.cipher))) {
            ssl->state &= ~SSL_CIPHER;
            ssl_debug_printf("ssl_set_master_secret can't find cipher suite 0x%X\n", ssl->session.cipher);
        } else {
            ssl->state |= SSL_CIPHER;
            ssl_debug_printf("ssl_set_master_secret set CIPHER 0x%04X -> state 0x%02X\n", ssl->session.cipher, ssl->state);
        }
    }

    /
    if (_client_random) {
        ssl_data_set(&ssl->client_random, _client_random, 32);
        ssl->state |= SSL_CLIENT_RANDOM;
        ssl_debug_printf("ssl_set_master_secret set CLIENT RANDOM -> state 0x%02X\n", ssl->state);
    }

    /
    if (_server_random) {
        ssl_data_set(&ssl->server_random, _server_random, 32);
        ssl->state |= SSL_SERVER_RANDOM;
        ssl_debug_printf("ssl_set_master_secret set SERVER RANDOM -> state 0x%02X\n", ssl->state);
    }

    /
    if (_master_secret) {
        ssl_data_set(&ssl->master_secret, _master_secret, 48);
        ssl->state |= SSL_MASTER_SECRET;
        ssl_debug_printf("ssl_set_master_secret set MASTER SECRET -> state 0x%02X\n", ssl->state);
    }

    ssl_debug_printf("ssl_set_master_secret trying to generate keys\n");
    if (ssl_generate_keyring_material(ssl)<0) {
        ssl_debug_printf("ssl_set_master_secret can't generate keyring material\n");
        return;
    }

    /
    ssl_change_cipher(ssl, TRUE);
    ssl_change_cipher(ssl, FALSE);

    /
    if (ssl->client && (client_seq != (guint32)-1)) {
        ssl->client->seq = client_seq;
        ssl_debug_printf("ssl_set_master_secret client->seq updated to %u\n", ssl->client->seq);
    }
    if (ssl->server && (server_seq != (guint32)-1)) {
        ssl->server->seq = server_seq;
        ssl_debug_printf("ssl_set_master_secret server->seq updated to %u\n", ssl->server->seq);
    }

    /
    iv_len = (ssl->cipher_suite->block>1) ? ssl->cipher_suite->block : 8;
    if (ssl->client && ((ssl->client->seq > 0) || (ssl->client_data_for_iv.data_len > iv_len))) {
        ssl_cipher_setiv(&ssl->client->evp, ssl->client_data_for_iv.data + ssl->client_data_for_iv.data_len - iv_len, iv_len);
        ssl_print_data("ssl_set_master_secret client IV updated",ssl->client_data_for_iv.data + ssl->client_data_for_iv.data_len - iv_len, iv_len);
    }
    if (ssl->server && ((ssl->server->seq > 0) || (ssl->server_data_for_iv.data_len > iv_len))) {
        ssl_cipher_setiv(&ssl->server->evp, ssl->server_data_for_iv.data + ssl->server_data_for_iv.data_len - iv_len, iv_len);
        ssl_print_data("ssl_set_master_secret server IV updated",ssl->server_data_for_iv.data + ssl->server_data_for_iv.data_len - iv_len, iv_len);
    }
}
