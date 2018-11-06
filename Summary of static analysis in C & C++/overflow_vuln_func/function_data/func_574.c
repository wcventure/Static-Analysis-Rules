void sha1_hmac_finish( sha1_hmac_context *hctx, guint8 digest[20] )
{
    guint8 tmpbuf[20];

    sha1_finish( &hctx->ctx, tmpbuf );

    sha1_starts( &hctx->ctx );
    sha1_update( &hctx->ctx, hctx->k_opad, 64 );
    sha1_update( &hctx->ctx, tmpbuf, 20 );
    sha1_finish( &hctx->ctx, digest );
}
