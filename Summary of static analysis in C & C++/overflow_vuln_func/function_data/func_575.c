void sha1_hmac( const guint8 *key, guint32 keylen, const guint8 *buf, guint32 buflen,
                guint8 digest[20] )
{
    sha1_hmac_context hctx;

    sha1_hmac_starts( &hctx, key, keylen );
    sha1_hmac_update( &hctx, buf, buflen );
    sha1_hmac_finish( &hctx, digest );
}
