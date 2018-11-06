static gint
ssl_cipher_init(gcry_cipher_hd_t *cipher, gint algo, guchar* sk,
        guchar* iv, gint mode)
{
    gint gcry_modes[]={GCRY_CIPHER_MODE_STREAM,GCRY_CIPHER_MODE_CBC,GCRY_CIPHER_MODE_CTR,GCRY_CIPHER_MODE_CTR,GCRY_CIPHER_MODE_CTR};
    gint err;
    if (algo == -1) {
        /
        *(cipher) = (gcry_cipher_hd_t)-1;
        return 0;
    }
    err = gcry_cipher_open(cipher, algo, gcry_modes[mode], 0);
    if (err !=0)
        return  -1;
    err = gcry_cipher_setkey(*(cipher), sk, gcry_cipher_get_algo_keylen (algo));
    if (err != 0)
        return -1;
    err = gcry_cipher_setiv(*(cipher), iv, gcry_cipher_get_algo_blklen (algo));
    if (err != 0)
        return -1;
    return 0;
}
