static void update_error_limit(WavpackFrameContext *ctx)
{
    int i, br[2], sl[2];

    for (i = 0; i <= ctx->stereo_in; i++) {
        ctx->ch[i].bitrate_acc += ctx->ch[i].bitrate_delta;
        br[i]                   = ctx->ch[i].bitrate_acc >> 16;
        sl[i]                   = LEVEL_DECAY(ctx->ch[i].slow_level);
    }
    if (ctx->stereo_in && ctx->hybrid_bitrate) {
        int balance = (sl[1] - sl[0] + br[1] + 1) >> 1;
        if (balance > br[0]) {
            br[1] = br[0] << 1;
            br[0] = 0;
        } else if (-balance > br[0]) {
            br[0] <<= 1;
            br[1]   = 0;
        } else {
            br[1] = br[0] + balance;
            br[0] = br[0] - balance;
        }
    }
    for (i = 0; i <= ctx->stereo_in; i++) {
        if (ctx->hybrid_bitrate) {
            if (sl[i] - br[i] > -0x100)
                ctx->ch[i].error_limit = wp_exp2(sl[i] - br[i] + 0x100);
            else
                ctx->ch[i].error_limit = 0;
        } else {
            ctx->ch[i].error_limit = wp_exp2(br[i]);
        }
    }
}
