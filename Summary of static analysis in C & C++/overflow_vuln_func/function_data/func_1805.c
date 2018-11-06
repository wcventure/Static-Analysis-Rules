static void
dissect_ssl2_hnd_client_hello(tvbuff_t *tvb,
                              proto_tree *tree, guint32 offset,
                              SslDecryptSession* ssl)
{
    /
    guint16 version;
    guint16 cipher_spec_length;
    guint16 session_id_length;
    guint16 challenge_length;

    proto_tree *ti;
    proto_tree *cs_tree;
    cs_tree=0;

    version = tvb_get_ntohs(tvb, offset);
    if (!ssl_is_valid_ssl_version(version))
    {
        /
        return;
    }

    if (tree || ssl)
    {
        /
        if (tree)
            proto_tree_add_item(tree, hf_ssl_handshake_client_version, tvb,
                            offset, 2, FALSE);
        offset += 2;

        cipher_spec_length = tvb_get_ntohs(tvb, offset);
        if (tree)
            proto_tree_add_item(tree, hf_ssl2_handshake_cipher_spec_len,
                            tvb, offset, 2, FALSE);
        offset += 2;

        session_id_length = tvb_get_ntohs(tvb, offset);
        if (tree)
            proto_tree_add_item(tree, hf_ssl2_handshake_session_id_len,
                            tvb, offset, 2, FALSE);
        offset += 2;

        challenge_length = tvb_get_ntohs(tvb, offset);
        if (tree)
            proto_tree_add_item(tree, hf_ssl2_handshake_challenge_len,
                            tvb, offset, 2, FALSE);
        offset += 2;

        if (tree)
        {
            /
            tvb_ensure_bytes_exist(tvb, offset, cipher_spec_length);
            ti = proto_tree_add_none_format(tree, hf_ssl_handshake_cipher_suites,
                                        tvb, offset, cipher_spec_length,
                                        "Cipher Specs (%u specs)",
                                        cipher_spec_length/3);

            /
            cs_tree = proto_item_add_subtree(ti, ett_ssl_cipher_suites);
            if (!cs_tree)
            {
                cs_tree = tree;     /
            }
        }

        /
        while (cipher_spec_length > 0)
        {
            if (cs_tree)
                proto_tree_add_item(cs_tree, hf_ssl2_handshake_cipher_spec,
                                tvb, offset, 3, FALSE);
            offset += 3;        /
            cipher_spec_length -= 3;
        }

        /
        if (session_id_length > 0)
        {
            if (tree)
            {
                tvb_ensure_bytes_exist(tvb, offset, session_id_length);
                proto_tree_add_bytes_format(tree,
                                             hf_ssl_handshake_session_id,
                                             tvb, offset, session_id_length,
                                             tvb_get_ptr(tvb, offset, session_id_length),
                                             "Session ID (%u byte%s)",
                                             session_id_length,
                                             plurality(session_id_length, "", "s"));
            }

            /
            if (ssl)
            {
                tvb_memcpy(tvb,ssl->session_id.data, offset, session_id_length);
                ssl->session_id.data_len = session_id_length;
                ssl->state &= ~(SSL_HAVE_SESSION_KEY|SSL_MASTER_SECRET|
                        SSL_CIPHER|SSL_SERVER_RANDOM);
            }
            offset += session_id_length;
        }

        /
        if (challenge_length > 0)
        {
            tvb_ensure_bytes_exist(tvb, offset, challenge_length);

            if (tree)
                proto_tree_add_item(tree, hf_ssl2_handshake_challenge,
                                tvb, offset, challenge_length, 0);
            if (ssl)
            {
                /
                gint max;
                max = challenge_length > 32? 32: challenge_length;

                ssl_debug_printf("client random len: %d padded to 32\n",
                    challenge_length);

                /
                memset(ssl->client_random.data, 0, 32 - max);
                tvb_memcpy(tvb, &ssl->client_random.data[32 - max], offset, max);
                ssl->client_random.data_len = 32;
                ssl->state |= SSL_CLIENT_RANDOM;

            }
            offset += challenge_length;
        }
    }
}
