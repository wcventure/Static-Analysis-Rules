static int smacker_decode_header_tree(SmackVContext *smk, BitstreamContext *bc,
                                      int **recodes, int *last, int size)
{
    int res;
    HuffContext huff;
    HuffContext tmp1, tmp2;
    VLC vlc[2] = { { 0 } };
    int escapes[3];
    DBCtx ctx;
    int err = 0;

    if(size >= UINT_MAX>>4){ // (((size + 3) >> 2) + 3) << 2 must not overflow
        av_log(smk->avctx, AV_LOG_ERROR, "size too large\n");
        return AVERROR_INVALIDDATA;
    }

    tmp1.length = 256;
    tmp1.maxlength = 0;
    tmp1.current = 0;
    tmp1.bits = av_mallocz(256 * 4);
    tmp1.lengths = av_mallocz(256 * sizeof(int));
    tmp1.values = av_mallocz(256 * sizeof(int));

    tmp2.length = 256;
    tmp2.maxlength = 0;
    tmp2.current = 0;
    tmp2.bits = av_mallocz(256 * 4);
    tmp2.lengths = av_mallocz(256 * sizeof(int));
    tmp2.values = av_mallocz(256 * sizeof(int));
    if (!tmp1.bits || !tmp1.lengths || !tmp1.values ||
        !tmp2.bits || !tmp2.lengths || !tmp2.values) {
        err = AVERROR(ENOMEM);
        goto error;
    }

    if (bitstream_read_bit(bc)) {
        smacker_decode_tree(bc, &tmp1, 0, 0);
        bitstream_skip(bc, 1);
        res = init_vlc(&vlc[0], SMKTREE_BITS, tmp1.length,
                    tmp1.lengths, sizeof(int), sizeof(int),
                    tmp1.bits, sizeof(uint32_t), sizeof(uint32_t), INIT_VLC_LE);
        if(res < 0) {
            av_log(smk->avctx, AV_LOG_ERROR, "Cannot build VLC table\n");
            err = res;
            goto error;
        }
    } else {
        av_log(smk->avctx, AV_LOG_ERROR, "Skipping low bytes tree\n");
    }
    if (bitstream_read_bit(bc)) {
        smacker_decode_tree(bc, &tmp2, 0, 0);
        bitstream_skip(bc, 1);
        res = init_vlc(&vlc[1], SMKTREE_BITS, tmp2.length,
                    tmp2.lengths, sizeof(int), sizeof(int),
                    tmp2.bits, sizeof(uint32_t), sizeof(uint32_t), INIT_VLC_LE);
        if(res < 0) {
            av_log(smk->avctx, AV_LOG_ERROR, "Cannot build VLC table\n");
            err = res;
            goto error;
        }
    } else {
        av_log(smk->avctx, AV_LOG_ERROR, "Skipping high bytes tree\n");
    }

    escapes[0]  = bitstream_read(bc, 8);
    escapes[0] |= bitstream_read(bc, 8) << 8;
    escapes[1]  = bitstream_read(bc, 8);
    escapes[1] |= bitstream_read(bc, 8) << 8;
    escapes[2]  = bitstream_read(bc, 8);
    escapes[2] |= bitstream_read(bc, 8) << 8;

    last[0] = last[1] = last[2] = -1;

    ctx.escapes[0] = escapes[0];
    ctx.escapes[1] = escapes[1];
    ctx.escapes[2] = escapes[2];
    ctx.v1 = &vlc[0];
    ctx.v2 = &vlc[1];
    ctx.recode1 = tmp1.values;
    ctx.recode2 = tmp2.values;
    ctx.last = last;

    huff.length = ((size + 3) >> 2) + 4;
    huff.maxlength = 0;
    huff.current = 0;
    huff.values = av_mallocz(huff.length * sizeof(int));
    if (!huff.values) {
        err = AVERROR(ENOMEM);
        goto error;
    }

    if ((res = smacker_decode_bigtree(bc, &huff, &ctx)) < 0)
        err = res;
    bitstream_skip(bc, 1);
    if(ctx.last[0] == -1) ctx.last[0] = huff.current++;
    if(ctx.last[1] == -1) ctx.last[1] = huff.current++;
    if(ctx.last[2] == -1) ctx.last[2] = huff.current++;
    if (ctx.last[0] >= huff.length ||
        ctx.last[1] >= huff.length ||
        ctx.last[2] >= huff.length) {
        av_log(smk->avctx, AV_LOG_ERROR, "Huffman codes out of range\n");
        err = AVERROR_INVALIDDATA;
    }

    *recodes = huff.values;

error:
    if(vlc[0].table)
        ff_free_vlc(&vlc[0]);
    if(vlc[1].table)
        ff_free_vlc(&vlc[1]);
    av_free(tmp1.bits);
    av_free(tmp1.lengths);
    av_free(tmp1.values);
    av_free(tmp2.bits);
    av_free(tmp2.lengths);
    av_free(tmp2.values);

    return err;
}
