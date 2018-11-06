static int smacker_decode_bigtree(BitstreamContext *bc, HuffContext *hc,
                                  DBCtx *ctx)
{
    if (hc->current + 1 >= hc->length) {
        av_log(NULL, AV_LOG_ERROR, "Tree size exceeded!\n");
        return AVERROR_INVALIDDATA;
    }
    if (!bitstream_read_bit(bc)) { // Leaf
        int val, i1, i2;
        i1 = ctx->v1->table ? bitstream_read_vlc(bc, ctx->v1->table, SMKTREE_BITS, 3) : 0;
        i2 = ctx->v2->table ? bitstream_read_vlc(bc, ctx->v2->table, SMKTREE_BITS, 3) : 0;
        if (i1 < 0 || i2 < 0)
            return AVERROR_INVALIDDATA;
        val = ctx->recode1[i1] | (ctx->recode2[i2] << 8);
        if(val == ctx->escapes[0]) {
            ctx->last[0] = hc->current;
            val = 0;
        } else if(val == ctx->escapes[1]) {
            ctx->last[1] = hc->current;
            val = 0;
        } else if(val == ctx->escapes[2]) {
            ctx->last[2] = hc->current;
            val = 0;
        }

        hc->values[hc->current++] = val;
        return 1;
    } else { //Node
        int r = 0, r_new, t;

        t = hc->current++;
        r = smacker_decode_bigtree(bc, hc, ctx);
        if(r < 0)
            return r;
        hc->values[t] = SMK_NODE | r;
        r++;
        r_new = smacker_decode_bigtree(bc, hc, ctx);
        if (r_new < 0)
            return r_new;
        return r + r_new;
    }
}
