int
ssl_decrypt_record(SslDecryptSession*ssl,SslDecoder* decoder, gint ct,
        const guchar* in, guint inl, StringInfo* comp_str, StringInfo* out_str, guint* outl)
{
    guint   pad, worklen, uncomplen;
    guint8 *mac;

    ssl_debug_printf("ssl_decrypt_record ciphertext len %d\n", inl);
    ssl_print_data("Ciphertext",in, inl);

    /
    if (inl > out_str->data_len)
    {
        ssl_debug_printf("ssl_decrypt_record: allocating %d bytes for decrypt data (old len %d)\n",
                inl + 32, out_str->data_len);
        ssl_data_realloc(out_str, inl + 32);
    }

    /

    /
    if (decoder->cipher_suite->mode == MODE_CBC) {
        switch (ssl->session.version) {
        case TLSV1DOT1_VERSION:
        case TLSV1DOT2_VERSION:
        case DTLSV1DOT0_VERSION:
        case DTLSV1DOT2_VERSION:
        case DTLSV1DOT0_OPENSSL_VERSION:
            if ((gint)inl < decoder->cipher_suite->block) {
                ssl_debug_printf("ssl_decrypt_record failed: input %d has no space for IV %d\n",
                        inl, decoder->cipher_suite->block);
                return -1;
            }
            pad = gcry_cipher_setiv(decoder->evp, in, decoder->cipher_suite->block);
            if (pad != 0) {
                ssl_debug_printf("ssl_decrypt_record failed: failed to set IV: %s %s\n",
                        gcry_strsource (pad), gcry_strerror (pad));
            }

            inl -= decoder->cipher_suite->block;
            in += decoder->cipher_suite->block;
            break;
        }
    }

    /
    if (decoder->cipher_suite->mode == MODE_GCM ||
        decoder->cipher_suite->mode == MODE_CCM ||
        decoder->cipher_suite->mode == MODE_CCM_8) {
        /
        guchar gcm_nonce[16] = { 0 };

        if ((gint)inl < SSL_EX_NONCE_LEN_GCM) {
            ssl_debug_printf("ssl_decrypt_record failed: input %d has no space for nonce %d\n",
                inl, SSL_EX_NONCE_LEN_GCM);
            return -1;
        }

        if (decoder->cipher_suite->mode == MODE_GCM) {
            memcpy(gcm_nonce, decoder->write_iv.data, decoder->write_iv.data_len); /
            memcpy(gcm_nonce + decoder->write_iv.data_len, in, SSL_EX_NONCE_LEN_GCM);
            /
            gcm_nonce[4 + SSL_EX_NONCE_LEN_GCM + 3] = 2;
        } else { /
            /
            /
            gcm_nonce[0] = 3 - 1;

            memcpy(gcm_nonce + 1, decoder->write_iv.data, decoder->write_iv.data_len); /
            memcpy(gcm_nonce + 1 + decoder->write_iv.data_len, in, SSL_EX_NONCE_LEN_GCM);
            gcm_nonce[4 + SSL_EX_NONCE_LEN_GCM + 3] = 1;
        }

        pad = gcry_cipher_setctr (decoder->evp, gcm_nonce, sizeof (gcm_nonce));
        if (pad != 0) {
            ssl_debug_printf("ssl_decrypt_record failed: failed to set CTR: %s %s\n",
                    gcry_strsource (pad), gcry_strerror (pad));
            return -1;
        }
        inl -= SSL_EX_NONCE_LEN_GCM;
        in += SSL_EX_NONCE_LEN_GCM;
    }

    /
    if ((pad = ssl_cipher_decrypt(&decoder->evp, out_str->data, out_str->data_len, in, inl))!= 0) {
        ssl_debug_printf("ssl_decrypt_record failed: ssl_cipher_decrypt: %s %s\n", gcry_strsource (pad),
                    gcry_strerror (pad));
        return -1;
    }

    ssl_print_data("Plaintext", out_str->data, inl);
    worklen=inl;

    /
    if (decoder->cipher_suite->mode == MODE_GCM ||
        decoder->cipher_suite->mode == MODE_CCM) {
        if (worklen < 16) {
            ssl_debug_printf("ssl_decrypt_record failed: missing tag, work %d\n", worklen);
            return -1;
        }
        /
        worklen -= 16;
    }
    /
    if (decoder->cipher_suite->mode == MODE_CCM_8) {
        if (worklen < 8) {
            ssl_debug_printf("ssl_decrypt_record failed: missing tag, work %d\n", worklen);
            return -1;
        }
        /
        worklen -= 8;
    }

    /
    if (decoder->cipher_suite->mode == MODE_CBC) {
        if (inl < 1) { /
            ssl_debug_printf("ssl_decrypt_record failed: input length %d too small\n", inl);
            return -1;
        }
        pad=out_str->data[inl-1];
        if (worklen <= pad) {
            ssl_debug_printf("ssl_decrypt_record failed: padding %d too large for work %d\n",
                pad, worklen);
            return -1;
        }
        worklen-=(pad+1);
        ssl_debug_printf("ssl_decrypt_record found padding %d final len %d\n",
            pad, worklen);
    }

    /
    if (decoder->cipher_suite->mode == MODE_STREAM ||
        decoder->cipher_suite->mode == MODE_CBC) {
        if (ssl_cipher_suite_dig(decoder->cipher_suite)->len > (gint)worklen) {
            ssl_debug_printf("ssl_decrypt_record wrong record len/padding outlen %d\n work %d\n",*outl, worklen);
            return -1;
        }
        worklen-=ssl_cipher_suite_dig(decoder->cipher_suite)->len;
        mac = out_str->data + worklen;
    } else / {
        /
        goto skip_mac;
    }

    /
    if (decoder->cipher_suite->mode == MODE_STREAM &&
            decoder->cipher_suite->enc == ENC_NULL &&
            !(ssl->state & SSL_MASTER_SECRET)) {
        ssl_debug_printf("MAC check skipped due to missing keys\n");
        goto skip_mac;
    }

    /
    ssl_debug_printf("checking mac (len %d, version %X, ct %d seq %d)\n",
        worklen, ssl->session.version, ct, decoder->seq);
    if(ssl->session.version==SSLV3_VERSION){
        if(ssl3_check_mac(decoder,ct,out_str->data,worklen,mac) < 0) {
            if(ssl_ignore_mac_failed) {
                ssl_debug_printf("ssl_decrypt_record: mac failed, but ignored for troubleshooting ;-)\n");
            }
            else{
                ssl_debug_printf("ssl_decrypt_record: mac failed\n");
                return -1;
            }
        }
        else{
            ssl_debug_printf("ssl_decrypt_record: mac ok\n");
        }
    }
    else if(ssl->session.version==TLSV1_VERSION || ssl->session.version==TLSV1DOT1_VERSION || ssl->session.version==TLSV1DOT2_VERSION){
        if(tls_check_mac(decoder,ct,ssl->session.version,out_str->data,worklen,mac)< 0) {
            if(ssl_ignore_mac_failed) {
                ssl_debug_printf("ssl_decrypt_record: mac failed, but ignored for troubleshooting ;-)\n");
            }
            else{
                ssl_debug_printf("ssl_decrypt_record: mac failed\n");
                return -1;
            }
        }
        else{
            ssl_debug_printf("ssl_decrypt_record: mac ok\n");
        }
    }
    else if(ssl->session.version==DTLSV1DOT0_VERSION ||
        ssl->session.version==DTLSV1DOT2_VERSION ||
        ssl->session.version==DTLSV1DOT0_OPENSSL_VERSION){
        /
        if(dtls_check_mac(decoder,ct,ssl->session.version,out_str->data,worklen,mac)>= 0) {
            ssl_debug_printf("ssl_decrypt_record: mac ok\n");
        }
        else if(tls_check_mac(decoder,ct,TLSV1_VERSION,out_str->data,worklen,mac)>= 0) {
            ssl_debug_printf("ssl_decrypt_record: dtls rfc-compliant mac failed, but old openssl's non-rfc-compliant mac ok\n");
        }
        else if(ssl_ignore_mac_failed) {
            ssl_debug_printf("ssl_decrypt_record: mac failed, but ignored for troubleshooting ;-)\n");
        }
        else{
            ssl_debug_printf("ssl_decrypt_record: mac failed\n");
            return -1;
        }
    }
skip_mac:

    *outl = worklen;

    if (decoder->compression > 0) {
        ssl_debug_printf("ssl_decrypt_record: compression method %d\n", decoder->compression);
        ssl_data_copy(comp_str, out_str);
        ssl_print_data("Plaintext compressed", comp_str->data, worklen);
        if (!decoder->decomp) {
            ssl_debug_printf("decrypt_ssl3_record: no decoder available\n");
            return -1;
        }
        if (ssl_decompress_record(decoder->decomp, comp_str->data, worklen, out_str, &uncomplen) < 0) return -1;
        ssl_print_data("Plaintext uncompressed", out_str->data, uncomplen);
        *outl = uncomplen;
    }

    return 0;
}
