static int upload_texture(SDL_Texture *tex, AVFrame *frame, struct SwsContext **img_convert_ctx) {
    int ret = 0;
    switch (frame->format) {
        case AV_PIX_FMT_YUV420P:
            if (frame->linesize[0] < 0 || frame->linesize[1] < 0 || frame->linesize[2] < 0) {
                av_log(NULL, AV_LOG_ERROR, "Negative linesize is not supported for YUV.\n");
                return -1;
            }
            ret = SDL_UpdateYUVTexture(tex, NULL, frame->data[0], frame->linesize[0],
                                                  frame->data[1], frame->linesize[1],
                                                  frame->data[2], frame->linesize[2]);
            break;
        case AV_PIX_FMT_BGRA:
            if (frame->linesize[0] < 0) {
                ret = SDL_UpdateTexture(tex, NULL, frame->data[0] + frame->linesize[0] * (frame->height - 1), -frame->linesize[0]);
            } else {
                ret = SDL_UpdateTexture(tex, NULL, frame->data[0], frame->linesize[0]);
            }
            break;
        default:
            /
            *img_convert_ctx = sws_getCachedContext(*img_convert_ctx,
                frame->width, frame->height, frame->format, frame->width, frame->height,
                AV_PIX_FMT_BGRA, sws_flags, NULL, NULL, NULL);
            if (*img_convert_ctx != NULL) {
                uint8_t *pixels;
                int pitch;
                if (!SDL_LockTexture(tex, NULL, (void **)&pixels, &pitch)) {
                    sws_scale(*img_convert_ctx, (const uint8_t * const *)frame->data, frame->linesize,
                              0, frame->height, &pixels, &pitch);
                    SDL_UnlockTexture(tex);
                }
            } else {
                av_log(NULL, AV_LOG_FATAL, "Cannot initialize the conversion context\n");
                ret = -1;
            }
            break;
    }
    return ret;
}
