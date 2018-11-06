static int subtitle_thread(void *arg)
{
    VideoState *is = arg;
    Frame *sp;
    int got_subtitle;
    double pts;
    int i, j;
    int r, g, b, y, u, v, a;

    for (;;) {
        while (is->paused && !is->subtitleq.abort_request) {
            SDL_Delay(10);
        }

        if (!(sp = frame_queue_peek_writable(&is->subpq)))
            return 0;

        if ((got_subtitle = decoder_decode_frame(&is->subdec, &sp->sub)) < 0)
            break;

        pts = 0;

        if (got_subtitle && sp->sub.format == 0) {
            if (sp->sub.pts != AV_NOPTS_VALUE)
                pts = sp->sub.pts / (double)AV_TIME_BASE;
            sp->pts = pts;
            sp->serial = is->subdec.pkt_serial;

            for (i = 0; i < sp->sub.num_rects; i++)
            {
                for (j = 0; j < sp->sub.rects[i]->nb_colors; j++)
                {
                    RGBA_IN(r, g, b, a, (uint32_t*)sp->sub.rects[i]->pict.data[1] + j);
                    y = RGB_TO_Y_CCIR(r, g, b);
                    u = RGB_TO_U_CCIR(r, g, b, 0);
                    v = RGB_TO_V_CCIR(r, g, b, 0);
                    YUVA_OUT((uint32_t*)sp->sub.rects[i]->pict.data[1] + j, y, u, v, a);
                }
            }

            /
            frame_queue_push(&is->subpq);
        } else if (got_subtitle) {
            avsubtitle_free(&sp->sub);
        }
    }
    return 0;
}
