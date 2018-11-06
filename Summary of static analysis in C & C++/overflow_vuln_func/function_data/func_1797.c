static gint
dtls_check_mac(SslDecoder*decoder, gint ct,int ver, guint8* data,
        guint32 datalen, guint8* mac)
{
    SSL_HMAC hm;
    gint     md;
    guint32  len;
    guint8   buf[20];
    gint16   temp;

    md=ssl_get_digest_by_name(ssl_cipher_suite_dig(decoder->cipher_suite)->name);
    ssl_debug_printf("dtls_check_mac mac type:%s md %d\n",
        ssl_cipher_suite_dig(decoder->cipher_suite)->name, md);

    if (ssl_hmac_init(&hm,decoder->mac_key.data,decoder->mac_key.data_len,md) != 0)
        return -1;
    ssl_debug_printf("dtls_check_mac seq: %d epoch: %d\n",decoder->seq,decoder->epoch);
    /
    fmt_seq(decoder->seq,buf);
    buf[0]=decoder->epoch>>8;
    buf[1]=(guint8)decoder->epoch;

    ssl_hmac_update(&hm,buf,8);

    /
    buf[0]=ct;
    ssl_hmac_update(&hm,buf,1);

    /
    temp = g_htons(ver);
    memcpy(buf, &temp, 2);
    ssl_hmac_update(&hm,buf,2);

    temp = g_htons(datalen);
    memcpy(buf, &temp, 2);
    ssl_hmac_update(&hm,buf,2);
    ssl_hmac_update(&hm,data,datalen);
    /
    ssl_hmac_final(&hm,buf,&len);
    ssl_hmac_cleanup(&hm);
    ssl_print_data("Mac", buf, len);
    if(memcmp(mac,buf,len))
        return -1;

    return(0);
}
