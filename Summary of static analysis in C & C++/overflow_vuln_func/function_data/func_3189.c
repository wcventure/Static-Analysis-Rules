static int decode_frame(AVCodecContext *avctx,
                            void *data, int *got_frame,
                            AVPacket *avpkt)
{
    AnsiContext *s = avctx->priv_data;
    uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    const uint8_t *buf_end   = buf+buf_size;
    int ret, i, count;

    if ((ret = ff_reget_buffer(avctx, s->frame)) < 0)
        return ret;
    if (!avctx->frame_number) {
        for (i=0; i<avctx->height; i++)
            memset(s->frame->data[0]+ i*s->frame->linesize[0], 0, avctx->width);
        memset(s->frame->data[1], 0, AVPALETTE_SIZE);
    }

    s->frame->pict_type           = AV_PICTURE_TYPE_I;
    s->frame->palette_has_changed = 1;
    set_palette((uint32_t *)s->frame->data[1]);
    if (!s->first_frame) {
        erase_screen(avctx);
        s->first_frame = 1;
    }

    while(buf < buf_end) {
        switch(s->state) {
        case STATE_NORMAL:
            switch (buf[0]) {
            case 0x00: //NUL
            case 0x07: //BEL
            case 0x1A: //SUB
                /
                break;
            case 0x08: //BS
                s->x = FFMAX(s->x - 1, 0);
                break;
            case 0x09: //HT
                i = s->x / FONT_WIDTH;
                count = ((i + 8) & ~7) - i;
                for (i = 0; i < count; i++)
                    draw_char(avctx, ' ');
                break;
            case 0x0A: //LF
                hscroll(avctx);
            case 0x0D: //CR
                s->x = 0;
                break;
            case 0x0C: //FF
                erase_screen(avctx);
                break;
            case 0x1B: //ESC
                s->state = STATE_ESCAPE;
                break;
            default:
                draw_char(avctx, buf[0]);
            }
            break;
        case STATE_ESCAPE:
            if (buf[0] == '[') {
                s->state   = STATE_CODE;
                s->nb_args = 0;
                s->args[0] = -1;
            } else {
                s->state = STATE_NORMAL;
                draw_char(avctx, 0x1B);
                continue;
            }
            break;
        case STATE_CODE:
            switch(buf[0]) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                if (s->nb_args < MAX_NB_ARGS)
                    s->args[s->nb_args] = FFMAX(s->args[s->nb_args], 0) * 10 + buf[0] - '0';
                break;
            case ';':
                s->nb_args++;
                if (s->nb_args < MAX_NB_ARGS)
                    s->args[s->nb_args] = 0;
                break;
            case 'M':
                s->state = STATE_MUSIC_PREAMBLE;
                break;
            case '=': case '?':
                /
                break;
            default:
                if (s->nb_args > MAX_NB_ARGS)
                    av_log(avctx, AV_LOG_WARNING, "args overflow (%i)\n", s->nb_args);
                if (s->nb_args < MAX_NB_ARGS && s->args[s->nb_args] >= 0)
                    s->nb_args++;
                if ((ret = execute_code(avctx, buf[0])) < 0)
                    return ret;
                s->state = STATE_NORMAL;
            }
            break;
        case STATE_MUSIC_PREAMBLE:
            if (buf[0] == 0x0E || buf[0] == 0x1B)
                s->state = STATE_NORMAL;
            /
            break;
        }
        buf++;
    }

    *got_frame = 1;
    if ((ret = av_frame_ref(data, s->frame)) < 0)
        return ret;
    return buf_size;
}
