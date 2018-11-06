static gint
tls_hash(StringInfo* secret, StringInfo* seed, gint md, StringInfo* out)
{
    guint8   *ptr;
    guint     left;
    gint      tocpy;
    guint8   *A;
    guint8    _A[48],tmp[48];
    guint     A_l,tmp_l;
    SSL_HMAC  hm;
    ptr  = out->data;
    left = out->data_len;


    ssl_print_string("tls_hash: hash secret", secret);
    ssl_print_string("tls_hash: hash seed", seed);
    A=seed->data;
    A_l=seed->data_len;

    while(left){
        ssl_hmac_init(&hm,secret->data,secret->data_len,md);
        ssl_hmac_update(&hm,A,A_l);
        ssl_hmac_final(&hm,_A,&A_l);
        ssl_hmac_cleanup(&hm);
        A=_A;

        ssl_hmac_init(&hm,secret->data,secret->data_len,md);
        ssl_hmac_update(&hm,A,A_l);
        ssl_hmac_update(&hm,seed->data,seed->data_len);
        ssl_hmac_final(&hm,tmp,&tmp_l);
        ssl_hmac_cleanup(&hm);

        tocpy=MIN(left,tmp_l);
        memcpy(ptr,tmp,tocpy);
        ptr+=tocpy;
        left-=tocpy;
    }

    ssl_print_string("hash out", out);
    return (0);
}
