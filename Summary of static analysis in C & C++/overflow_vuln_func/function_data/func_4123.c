static void dvbsub_parse_pixel_data_block(AVCodecContext *avctx, DVBSubObjectDisplay *display,
                                          const uint8_t *buf, int buf_size, int top_bottom, int non_mod)
{
    DVBSubContext *ctx = avctx->priv_data;

    DVBSubRegion *region = get_region(ctx, display->region_id);
    const uint8_t *buf_end = buf + buf_size;
    uint8_t *pbuf;
    int x_pos, y_pos;
    int i;

    uint8_t map2to4[] = { 0x0,  0x7,  0x8,  0xf};
    uint8_t map2to8[] = {0x00, 0x77, 0x88, 0xff};
    uint8_t map4to8[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                         0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    uint8_t *map_table;

    av_dlog(avctx, "DVB pixel block size %d, %s field:\n", buf_size,
            top_bottom ? "bottom" : "top");

#ifdef DEBUG_PACKET_CONTENTS
    for (i = 0; i < buf_size; i++) {
        if (i % 16 == 0)
            av_log(avctx, AV_LOG_INFO, "0x%08p: ", buf+i);

        av_log(avctx, AV_LOG_INFO, "%02x ", buf[i]);
        if (i % 16 == 15)
            av_log(avctx, AV_LOG_INFO, "\n");
    }

    if (i % 16)
        av_log(avctx, AV_LOG_INFO, "\n");

#endif

    if (region == 0)
        return;

    pbuf = region->pbuf;

    x_pos = display->x_pos;
    y_pos = display->y_pos;

    if ((y_pos & 1) != top_bottom)
        y_pos++;

    while (buf < buf_end) {
        if (x_pos > region->width || y_pos > region->height) {
            av_log(avctx, AV_LOG_ERROR, "Invalid object location!\n");
            return;
        }

        switch (*buf++) {
        case 0x10:
            if (region->depth == 8)
                map_table = map2to8;
            else if (region->depth == 4)
                map_table = map2to4;
            else
                map_table = NULL;

            x_pos += dvbsub_read_2bit_string(pbuf + (y_pos * region->width) + x_pos,
                                                region->width - x_pos, &buf, buf_size,
                                                non_mod, map_table);
            break;
        case 0x11:
            if (region->depth < 4) {
                av_log(avctx, AV_LOG_ERROR, "4-bit pixel string in %d-bit region!\n", region->depth);
                return;
            }

            if (region->depth == 8)
                map_table = map4to8;
            else
                map_table = NULL;

            x_pos += dvbsub_read_4bit_string(pbuf + (y_pos * region->width) + x_pos,
                                                region->width - x_pos, &buf, buf_size,
                                                non_mod, map_table);
            break;
        case 0x12:
            if (region->depth < 8) {
                av_log(avctx, AV_LOG_ERROR, "8-bit pixel string in %d-bit region!\n", region->depth);
                return;
            }

            x_pos += dvbsub_read_8bit_string(pbuf + (y_pos * region->width) + x_pos,
                                                region->width - x_pos, &buf, buf_size,
                                                non_mod, NULL);
            break;

        case 0x20:
            map2to4[0] = (*buf) >> 4;
            map2to4[1] = (*buf++) & 0xf;
            map2to4[2] = (*buf) >> 4;
            map2to4[3] = (*buf++) & 0xf;
            break;
        case 0x21:
            for (i = 0; i < 4; i++)
                map2to8[i] = *buf++;
            break;
        case 0x22:
            for (i = 0; i < 16; i++)
                map4to8[i] = *buf++;
            break;

        case 0xf0:
            x_pos = display->x_pos;
            y_pos += 2;
            break;
        default:
            av_log(avctx, AV_LOG_INFO, "Unknown/unsupported pixel block 0x%x\n", *(buf-1));
        }
    }

}
