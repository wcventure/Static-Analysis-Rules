static void init_entropy_decoder(APEContext *ctx)
{
    /
    ctx->CRC = bytestream_get_be32(&ctx->ptr);

    /
    ctx->frameflags = 0;
    if ((ctx->fileversion > 3820) && (ctx->CRC & 0x80000000)) {
        ctx->CRC &= ~0x80000000;

        ctx->frameflags = bytestream_get_be32(&ctx->ptr);
    }

    /
    ctx->blocksdecoded = 0;

    /
    ctx->riceX.k = 10;
    ctx->riceX.ksum = (1 << ctx->riceX.k) * 16;
    ctx->riceY.k = 10;
    ctx->riceY.ksum = (1 << ctx->riceY.k) * 16;

    /
    ctx->ptr++;

    range_start_decoding(ctx);
}
