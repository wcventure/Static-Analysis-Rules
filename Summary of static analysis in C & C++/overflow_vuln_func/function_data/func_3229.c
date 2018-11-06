static int dnxhd_encode_rdo(AVCodecContext *avctx, DNXHDEncContext *ctx)
{
    int lambda, up_step, down_step;
    int last_lower = INT_MAX, last_higher = 0;
    int x, y, q;

    for (q = 1; q < avctx->qmax; q++) {
        ctx->qscale = q;
        avctx->execute2(avctx, dnxhd_calc_bits_thread, NULL, NULL, ctx->m.mb_height);
    }
    up_step = down_step = 2<<LAMBDA_FRAC_BITS;
    lambda = ctx->lambda;

    for (;;) {
        int bits = 0;
        int end = 0;
        if (lambda == last_higher) {
            lambda++;
            end = 1; // need to set final qscales/bits
        }
        for (y = 0; y < ctx->m.mb_height; y++) {
            for (x = 0; x < ctx->m.mb_width; x++) {
                unsigned min = UINT_MAX;
                int qscale = 1;
                int mb = y*ctx->m.mb_width+x;
                for (q = 1; q < avctx->qmax; q++) {
                    unsigned score = ctx->mb_rc[q][mb].bits*lambda+(ctx->mb_rc[q][mb].ssd<<LAMBDA_FRAC_BITS);
                    if (score < min) {
                        min = score;
                        qscale = q;
                    }
                }
                bits += ctx->mb_rc[qscale][mb].bits;
                ctx->mb_qscale[mb] = qscale;
                ctx->mb_bits[mb] = ctx->mb_rc[qscale][mb].bits;
            }
            bits = (bits+31)&~31; // padding
            if (bits > ctx->frame_bits)
                break;
        }
        //av_dlog(ctx->m.avctx, "lambda %d, up %u, down %u, bits %d, frame %d\n",
        //        lambda, last_higher, last_lower, bits, ctx->frame_bits);
        if (end) {
            if (bits > ctx->frame_bits)
                return -1;
            break;
        }
        if (bits < ctx->frame_bits) {
            last_lower = FFMIN(lambda, last_lower);
            if (last_higher != 0)
                lambda = (lambda+last_higher)>>1;
            else
                lambda -= down_step;
            down_step *= 5; // XXX tune ?
            up_step = 1<<LAMBDA_FRAC_BITS;
            lambda = FFMAX(1, lambda);
            if (lambda == last_lower)
                break;
        } else {
            last_higher = FFMAX(lambda, last_higher);
            if (last_lower != INT_MAX)
                lambda = (lambda+last_lower)>>1;
            else if ((int64_t)lambda + up_step > INT_MAX)
                return -1;
            else
                lambda += up_step;
            up_step = FFMIN((int64_t)up_step*5, INT_MAX);
            down_step = 1<<LAMBDA_FRAC_BITS;
        }
    }
    //av_dlog(ctx->m.avctx, "out lambda %d\n", lambda);
    ctx->lambda = lambda;
    return 0;
}
