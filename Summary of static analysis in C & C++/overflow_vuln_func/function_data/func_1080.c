int
ssl_generate_keyring_material(SslDecryptSession*ssl_session)
{
    StringInfo  key_block = { NULL, 0 };
    guint8      _iv_c[MAX_BLOCK_SIZE],_iv_s[MAX_BLOCK_SIZE];
    guint8      _key_c[MAX_KEY_SIZE],_key_s[MAX_KEY_SIZE];
    gint        needed;
    guint8     *ptr, *c_iv = _iv_c,*s_iv = _iv_s;
    guint8     *c_wk = NULL, *s_wk = NULL, *c_mk = NULL, *s_mk = NULL;
    const SslCipherSuite *cipher_suite = ssl_session->cipher_suite;

    /
    guint need_all = SSL_CIPHER|SSL_CLIENT_RANDOM|SSL_SERVER_RANDOM|SSL_VERSION;
    guint need_any = SSL_MASTER_SECRET | SSL_PRE_MASTER_SECRET;
    if (((ssl_session->state & need_all) != need_all) || ((ssl_session->state & need_any) == 0)) {
        ssl_debug_printf("ssl_generate_keyring_material not enough data to generate key "
                         "(0x%02X required 0x%02X or 0x%02X)\n", ssl_session->state,
                         need_all|SSL_MASTER_SECRET, need_all|SSL_PRE_MASTER_SECRET);
        /
        need_all = SSL_CIPHER|SSL_VERSION;
        if ((ssl_session->state & need_all) == need_all &&
                cipher_suite->enc == ENC_NULL) {
            ssl_debug_printf("%s NULL cipher found, will create a decoder but "
                    "skip MAC validation as keys are missing.\n", G_STRFUNC);
            goto create_decoders;
        }

        return -1;
    }

    /
    if (!(ssl_session->state & SSL_MASTER_SECRET)) {
        if ((ssl_session->state & SSL_EXTENDED_MASTER_SECRET_MASK) == SSL_EXTENDED_MASTER_SECRET_MASK) {
            StringInfo handshake_hashed_data;
            gint ret;

            handshake_hashed_data.data = NULL;
            handshake_hashed_data.data_len = 0;

            ssl_debug_printf("%s:PRF(pre_master_secret_extended)\n", G_STRFUNC);
            ssl_print_string("pre master secret",&ssl_session->pre_master_secret);
            DISSECTOR_ASSERT(ssl_session->handshake_data.data_len > 0);

            switch(ssl_session->session.version) {
            case TLSV1_VERSION:
            case TLSV1DOT1_VERSION:
            case DTLSV1DOT0_VERSION:
            case DTLSV1DOT0_OPENSSL_VERSION:
                ret = tls_handshake_hash(ssl_session, &handshake_hashed_data);
                break;
            default:
                switch (cipher_suite->dig) {
                case DIG_SHA384:
                    ret = tls12_handshake_hash(ssl_session, GCRY_MD_SHA384, &handshake_hashed_data);
                    break;
                default:
                    ret = tls12_handshake_hash(ssl_session, GCRY_MD_SHA256, &handshake_hashed_data);
                    break;
                }
                break;
            }
            if (ret) {
                ssl_debug_printf("%s can't generate handshake hash\n", G_STRFUNC);
                return -1;
            }

            wmem_free(wmem_file_scope(), ssl_session->handshake_data.data);
            ssl_session->handshake_data.data = NULL;
            ssl_session->handshake_data.data_len = 0;

            if (!prf(ssl_session, &ssl_session->pre_master_secret, "extended master secret",
                     &handshake_hashed_data,
                     NULL, &ssl_session->master_secret,
                     SSL_MASTER_SECRET_LENGTH)) {
                ssl_debug_printf("%s can't generate master_secret\n", G_STRFUNC);
                g_free(handshake_hashed_data.data);
                return -1;
            }
            g_free(handshake_hashed_data.data);
        } else {
            ssl_debug_printf("%s:PRF(pre_master_secret)\n", G_STRFUNC);
            ssl_print_string("pre master secret",&ssl_session->pre_master_secret);
            ssl_print_string("client random",&ssl_session->client_random);
            ssl_print_string("server random",&ssl_session->server_random);
            if (!prf(ssl_session, &ssl_session->pre_master_secret, "master secret",
                     &ssl_session->client_random,
                     &ssl_session->server_random, &ssl_session->master_secret,
                     SSL_MASTER_SECRET_LENGTH)) {
                ssl_debug_printf("%s can't generate master_secret\n", G_STRFUNC);
                return -1;
            }
        }
        ssl_print_string("master secret",&ssl_session->master_secret);

        /
        ssl_session->state &= ~SSL_PRE_MASTER_SECRET;
        ssl_session->state |= SSL_MASTER_SECRET;
    }

    /
    needed=ssl_cipher_suite_dig(cipher_suite)->len*2;
    needed+=cipher_suite->bits / 4;
    if(cipher_suite->block>1)
        needed+=cipher_suite->block*2;

    key_block.data = (guchar *)g_malloc(needed);
    ssl_debug_printf("%s sess key generation\n", G_STRFUNC);
    if (!prf(ssl_session, &ssl_session->master_secret, "key expansion",
            &ssl_session->server_random,&ssl_session->client_random,
            &key_block, needed)) {
        ssl_debug_printf("%s can't generate key_block\n", G_STRFUNC);
        goto fail;
    }
    ssl_print_string("key expansion", &key_block);

    ptr=key_block.data;
    /
    if (cipher_suite->mode == MODE_GCM ||
        cipher_suite->mode == MODE_CCM ||
        cipher_suite->mode == MODE_CCM_8) {
        c_mk = s_mk = NULL;
    } else {
        c_mk=ptr; ptr+=ssl_cipher_suite_dig(cipher_suite)->len;
        s_mk=ptr; ptr+=ssl_cipher_suite_dig(cipher_suite)->len;
    }

    c_wk=ptr; ptr+=cipher_suite->eff_bits/8;
    s_wk=ptr; ptr+=cipher_suite->eff_bits/8;

    if(cipher_suite->block>1){
        c_iv=ptr; ptr+=cipher_suite->block;
        s_iv=ptr; /
    }

    /
    if (cipher_suite->eff_bits < cipher_suite->bits) {
        if(cipher_suite->block>1){

            /
            if(cipher_suite->block>MAX_BLOCK_SIZE) {
                ssl_debug_printf("%s cipher suite block must be at most %d nut is %d\n",
                G_STRFUNC, MAX_BLOCK_SIZE, cipher_suite->block);
                goto fail;
            }

            if(ssl_session->session.version==SSLV3_VERSION){
                /
                StringInfo iv_c, iv_s;
                iv_c.data = _iv_c;
                iv_s.data = _iv_s;

                ssl_debug_printf("%s ssl3_generate_export_iv\n", G_STRFUNC);
                ssl3_generate_export_iv(&ssl_session->client_random,
                        &ssl_session->server_random, &iv_c,
                        cipher_suite->block);
                ssl_debug_printf("%s ssl3_generate_export_iv(2)\n", G_STRFUNC);
                ssl3_generate_export_iv(&ssl_session->server_random,
                        &ssl_session->client_random, &iv_s,
                        cipher_suite->block);
            }
            else{
                guint8 _iv_block[MAX_BLOCK_SIZE * 2];
                StringInfo iv_block;
                StringInfo key_null;
                guint8 _key_null;

                key_null.data = &_key_null;
                key_null.data_len = 0;

                iv_block.data = _iv_block;

                ssl_debug_printf("%s prf(iv_block)\n", G_STRFUNC);
                if (!prf(ssl_session, &key_null, "IV block",
                        &ssl_session->client_random,
                        &ssl_session->server_random, &iv_block,
                        cipher_suite->block * 2)) {
                    ssl_debug_printf("%s can't generate tls31 iv block\n", G_STRFUNC);
                    goto fail;
                }

                memcpy(_iv_c,iv_block.data,cipher_suite->block);
                memcpy(_iv_s,iv_block.data+cipher_suite->block,
                    cipher_suite->block);
            }

            c_iv=_iv_c;
            s_iv=_iv_s;
        }

        if (ssl_session->session.version==SSLV3_VERSION){

            SSL_MD5_CTX md5;
            ssl_debug_printf("%s MD5(client_random)\n", G_STRFUNC);

            ssl_md5_init(&md5);
            ssl_md5_update(&md5,c_wk,cipher_suite->eff_bits/8);
            ssl_md5_update(&md5,ssl_session->client_random.data,
                ssl_session->client_random.data_len);
            ssl_md5_update(&md5,ssl_session->server_random.data,
                ssl_session->server_random.data_len);
            ssl_md5_final(_key_c,&md5);
            ssl_md5_cleanup(&md5);
            c_wk=_key_c;

            ssl_md5_init(&md5);
            ssl_debug_printf("%s MD5(server_random)\n", G_STRFUNC);
            ssl_md5_update(&md5,s_wk,cipher_suite->eff_bits/8);
            ssl_md5_update(&md5,ssl_session->server_random.data,
                ssl_session->server_random.data_len);
            ssl_md5_update(&md5,ssl_session->client_random.data,
                ssl_session->client_random.data_len);
            ssl_md5_final(_key_s,&md5);
            ssl_md5_cleanup(&md5);
            s_wk=_key_s;
        }
        else{
            StringInfo key_c, key_s, k;
            key_c.data = _key_c;
            key_s.data = _key_s;

            k.data = c_wk;
            k.data_len = cipher_suite->eff_bits/8;
            ssl_debug_printf("%s PRF(key_c)\n", G_STRFUNC);
            if (!prf(ssl_session, &k, "client write key",
                    &ssl_session->client_random,
                    &ssl_session->server_random, &key_c, sizeof(_key_c))) {
                ssl_debug_printf("%s can't generate tll31 server key \n", G_STRFUNC);
                goto fail;
            }
            c_wk=_key_c;

            k.data = s_wk;
            k.data_len = cipher_suite->eff_bits/8;
            ssl_debug_printf("%s PRF(key_s)\n", G_STRFUNC);
            if (!prf(ssl_session, &k, "server write key",
                    &ssl_session->client_random,
                    &ssl_session->server_random, &key_s, sizeof(_key_s))) {
                ssl_debug_printf("%s can't generate tll31 client key \n", G_STRFUNC);
                goto fail;
            }
            s_wk=_key_s;
        }
    }

    /
    if (c_mk != NULL) {
        ssl_print_data("Client MAC key",c_mk,ssl_cipher_suite_dig(cipher_suite)->len);
        ssl_print_data("Server MAC key",s_mk,ssl_cipher_suite_dig(cipher_suite)->len);
    }
    ssl_print_data("Client Write key",c_wk,cipher_suite->bits/8);
    ssl_print_data("Server Write key",s_wk,cipher_suite->bits/8);

    if(cipher_suite->block>1) {
        ssl_print_data("Client Write IV",c_iv,cipher_suite->block);
        ssl_print_data("Server Write IV",s_iv,cipher_suite->block);
    }
    else {
        ssl_print_data("Client Write IV",c_iv,8);
        ssl_print_data("Server Write IV",s_iv,8);
    }

create_decoders:
    /
    ssl_debug_printf("%s ssl_create_decoder(client)\n", G_STRFUNC);
    ssl_session->client_new = ssl_create_decoder(cipher_suite, ssl_session->session.compression, c_mk, c_wk, c_iv);
    if (!ssl_session->client_new) {
        ssl_debug_printf("%s can't init client decoder\n", G_STRFUNC);
        goto fail;
    }
    ssl_debug_printf("%s ssl_create_decoder(server)\n", G_STRFUNC);
    ssl_session->server_new = ssl_create_decoder(cipher_suite, ssl_session->session.compression, s_mk, s_wk, s_iv);
    if (!ssl_session->server_new) {
        ssl_debug_printf("%s can't init client decoder\n", G_STRFUNC);
        goto fail;
    }

    /
    ssl_session->client_new->flow = ssl_session->client ? ssl_session->client->flow : ssl_create_flow();
    ssl_session->server_new->flow = ssl_session->server ? ssl_session->server->flow : ssl_create_flow();

    ssl_debug_printf("%s: client seq %d, server seq %d\n",
        G_STRFUNC, ssl_session->client_new->seq, ssl_session->server_new->seq);
    g_free(key_block.data);
    ssl_session->state |= SSL_HAVE_SESSION_KEY;
    return 0;

fail:
    g_free(key_block.data);
    return -1;
}
