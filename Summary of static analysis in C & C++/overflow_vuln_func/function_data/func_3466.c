static void video_image_display(VideoState *is)
{
    Frame *vp;
    Frame *sp = NULL;
    SDL_Rect rect;

    vp = frame_queue_peek_last(&is->pictq);
    if (vp->bmp) {
        if (is->subtitle_st) {
            if (frame_queue_nb_remaining(&is->subpq) > 0) {
                sp = frame_queue_peek(&is->subpq);

                if (vp->pts >= sp->pts + ((float) sp->sub.start_display_time / 1000)) {
                    if (!sp->uploaded) {
                        uint8_t *pixels;
                        int pitch;
                        int i;
                        if (!sp->width || !sp->height) {
                            sp->width = vp->width;
                            sp->height = vp->height;
                        }
                        if (realloc_texture(&is->sub_texture, SDL_PIXELFORMAT_ARGB8888, sp->width, sp->height, SDL_BLENDMODE_BLEND, 1) < 0)
                            return;

                        for (i = 0; i < sp->sub.num_rects; i++) {
                            AVSubtitleRect *sub_rect = sp->sub.rects[i];

                            sub_rect->x = av_clip(sub_rect->x, 0, sp->width );
                            sub_rect->y = av_clip(sub_rect->y, 0, sp->height);
                            sub_rect->w = av_clip(sub_rect->w, 0, sp->width  - sub_rect->x);
                            sub_rect->h = av_clip(sub_rect->h, 0, sp->height - sub_rect->y);

                            is->sub_convert_ctx = sws_getCachedContext(is->sub_convert_ctx,
                                sub_rect->w, sub_rect->h, AV_PIX_FMT_PAL8,
                                sub_rect->w, sub_rect->h, AV_PIX_FMT_BGRA,
                                0, NULL, NULL, NULL);
                            if (!is->sub_convert_ctx) {
                                av_log(NULL, AV_LOG_FATAL, "Cannot initialize the conversion context\n");
                                return;
                            }
                            if (!SDL_LockTexture(is->sub_texture, (SDL_Rect *)sub_rect, (void **)&pixels, &pitch)) {
                                sws_scale(is->sub_convert_ctx, (const uint8_t * const *)sub_rect->data, sub_rect->linesize,
                                          0, sub_rect->h, &pixels, &pitch);
                                SDL_UnlockTexture(is->sub_texture);
                            }
                        }
                        sp->uploaded = 1;
                    }
                } else
                    sp = NULL;
            }
        }

        calculate_display_rect(&rect, is->xleft, is->ytop, is->width, is->height, vp->width, vp->height, vp->sar);

        if (!vp->uploaded) {
            if (upload_texture(vp->bmp, vp->frame, &is->img_convert_ctx) < 0)
                return;
            vp->uploaded = 1;
            vp->flip_v = vp->frame->linesize[0] < 0;
        }

        SDL_RenderCopyEx(renderer, vp->bmp, NULL, &rect, 0, NULL, vp->flip_v ? SDL_FLIP_VERTICAL : 0);
        if (sp) {
#if USE_ONEPASS_SUBTITLE_RENDER
            SDL_RenderCopy(renderer, is->sub_texture, NULL, &rect);
#else
            int i;
            double xratio = (double)rect.w / (double)sp->width;
            double yratio = (double)rect.h / (double)sp->height;
            for (i = 0; i < sp->sub.num_rects; i++) {
                SDL_Rect *sub_rect = (SDL_Rect*)sp->sub.rects[i];
                SDL_Rect target = {.x = rect.x + sub_rect->x * xratio,
                                   .y = rect.y + sub_rect->y * yratio,
                                   .w = sub_rect->w * xratio,
                                   .h = sub_rect->h * yratio};
                SDL_RenderCopy(renderer, is->sub_texture, sub_rect, &target);
            }
#endif
        }
    }
}
