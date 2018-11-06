static SslDecoder*
ssl_create_decoder(const SslCipherSuite *cipher_suite, gint compression,
        guint8 *mk, guint8 *sk, guint8 *iv)
{
    SslDecoder *dec;
    gint        ciph;

    dec = (SslDecoder *)wmem_alloc0(wmem_file_scope(), sizeof(SslDecoder));
    /
    if(cipher_suite->enc!=ENC_NULL) {
        ssl_debug_printf("ssl_create_decoder CIPHER: %s\n", ciphers[cipher_suite->enc-0x30]);
        ciph=ssl_get_cipher_by_name(ciphers[cipher_suite->enc-0x30]);
    } else {
        ssl_debug_printf("ssl_create_decoder CIPHER: %s\n", "NULL");
        ciph = -1;
    }
    if (ciph == 0) {
        ssl_debug_printf("ssl_create_decoder can't find cipher %s\n",
            ciphers[cipher_suite->enc > ENC_NULL ? ENC_NULL-0x30 : (cipher_suite->enc-0x30)]);
        return NULL;
    }

    /
    dec->cipher_suite=cipher_suite;
    dec->compression = compression;
    /
    if (mk != NULL) {
        dec->mac_key.data = dec->_mac_key_or_write_iv;
        ssl_data_set(&dec->mac_key, mk, ssl_cipher_suite_dig(cipher_suite)->len);
    } else if (iv != NULL) {
        dec->write_iv.data = dec->_mac_key_or_write_iv;
        ssl_data_set(&dec->write_iv, iv, cipher_suite->block);
    } else {
        DISSECTOR_ASSERT(cipher_suite->enc == ENC_NULL);
    }
    dec->seq = 0;
    dec->decomp = ssl_create_decompressor(compression);

    /
    if (dec->evp)
        ssl_cipher_cleanup(&dec->evp);

    if (ssl_cipher_init(&dec->evp,ciph,sk,iv,cipher_suite->mode) < 0) {
        ssl_debug_printf("ssl_create_decoder: can't create cipher id:%d mode:%d\n",
            ciph, cipher_suite->mode);
        return NULL;
    }

    ssl_debug_printf("decoder initialized (digest len %d)\n", ssl_cipher_suite_dig(cipher_suite)->len);
    return dec;
}
