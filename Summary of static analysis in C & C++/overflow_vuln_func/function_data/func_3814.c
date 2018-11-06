static int decode_frame(AVCodecContext *avctx, void *data, int *got_frame,
                        AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size       = avpkt->size;
    VmncContext * const c = avctx->priv_data;
    GetByteContext *gb = &c->gb;
    uint8_t *outptr;
    int dx, dy, w, h, depth, enc, chunks, res, size_left, ret;

    if ((ret = ff_reget_buffer(avctx, c->pic)) < 0)
        return ret;

    bytestream2_init(gb, buf, buf_size);

    c->pic->key_frame = 0;
    c->pic->pict_type = AV_PICTURE_TYPE_P;

    // restore screen after cursor
    if (c->screendta) {
        int i;
        w = c->cur_w;
        if (c->width < c->cur_x + w)
            w = c->width - c->cur_x;
        h = c->cur_h;
        if (c->height < c->cur_y + h)
            h = c->height - c->cur_y;
        dx = c->cur_x;
        if (dx < 0) {
            w += dx;
            dx = 0;
        }
        dy = c->cur_y;
        if (dy < 0) {
            h += dy;
            dy = 0;
        }
        if ((w > 0) && (h > 0)) {
            outptr = c->pic->data[0] + dx * c->bpp2 + dy * c->pic->linesize[0];
            for (i = 0; i < h; i++) {
                memcpy(outptr, c->screendta + i * c->cur_w * c->bpp2,
                       w * c->bpp2);
                outptr += c->pic->linesize[0];
            }
        }
    }
    bytestream2_skip(gb, 2);
    chunks = bytestream2_get_be16(gb);
    while (chunks--) {
        if (bytestream2_get_bytes_left(gb) < 12) {
            av_log(avctx, AV_LOG_ERROR, "Premature end of data!\n");
            return -1;
        }
        dx  = bytestream2_get_be16(gb);
        dy  = bytestream2_get_be16(gb);
        w   = bytestream2_get_be16(gb);
        h   = bytestream2_get_be16(gb);
        enc = bytestream2_get_be32(gb);
        outptr = c->pic->data[0] + dx * c->bpp2 + dy * c->pic->linesize[0];
        size_left = bytestream2_get_bytes_left(gb);
        switch (enc) {
        case MAGIC_WMVd: // cursor
            if (w*(int64_t)h*c->bpp2 > INT_MAX/2 - 2) {
                av_log(avctx, AV_LOG_ERROR, "dimensions too large\n");
                return AVERROR_INVALIDDATA;
            }
            if (size_left < 2 + w * h * c->bpp2 * 2) {
                av_log(avctx, AV_LOG_ERROR,
                       "Premature end of data! (need %i got %i)\n",
                       2 + w * h * c->bpp2 * 2, size_left);
                return AVERROR_INVALIDDATA;
            }
            bytestream2_skip(gb, 2);
            c->cur_w  = w;
            c->cur_h  = h;
            c->cur_hx = dx;
            c->cur_hy = dy;
            if ((c->cur_hx > c->cur_w) || (c->cur_hy > c->cur_h)) {
                av_log(avctx, AV_LOG_ERROR,
                       "Cursor hot spot is not in image: "
                       "%ix%i of %ix%i cursor size\n",
                       c->cur_hx, c->cur_hy, c->cur_w, c->cur_h);
                c->cur_hx = c->cur_hy = 0;
            }
            if (c->cur_w * c->cur_h >= INT_MAX / c->bpp2) {
                reset_buffers(c);
                return AVERROR(EINVAL);
            } else {
                int screen_size = c->cur_w * c->cur_h * c->bpp2;
                if ((ret = av_reallocp(&c->curbits, screen_size)) < 0 ||
                    (ret = av_reallocp(&c->curmask, screen_size)) < 0 ||
                    (ret = av_reallocp(&c->screendta, screen_size)) < 0) {
                    reset_buffers(c);
                    return ret;
                }
            }
            load_cursor(c);
            break;
        case MAGIC_WMVe: // unknown
            bytestream2_skip(gb, 2);
            break;
        case MAGIC_WMVf: // update cursor position
            c->cur_x = dx - c->cur_hx;
            c->cur_y = dy - c->cur_hy;
            break;
        case MAGIC_WMVg: // unknown
            bytestream2_skip(gb, 10);
            break;
        case MAGIC_WMVh: // unknown
            bytestream2_skip(gb, 4);
            break;
        case MAGIC_WMVi: // ServerInitialization struct
            c->pic->key_frame = 1;
            c->pic->pict_type = AV_PICTURE_TYPE_I;
            depth = bytestream2_get_byte(gb);
            if (depth != c->bpp) {
                av_log(avctx, AV_LOG_INFO,
                       "Depth mismatch. Container %i bpp, "
                       "Frame data: %i bpp\n",
                       c->bpp, depth);
            }
            bytestream2_skip(gb, 1);
            c->bigendian = bytestream2_get_byte(gb);
            if (c->bigendian & (~1)) {
                av_log(avctx, AV_LOG_INFO,
                       "Invalid header: bigendian flag = %i\n", c->bigendian);
                return AVERROR_INVALIDDATA;
            }
            //skip the rest of pixel format data
            bytestream2_skip(gb, 13);
            break;
        case MAGIC_WMVj: // unknown
            bytestream2_skip(gb, 2);
            break;
        case 0x00000000: // raw rectangle data
            if ((dx + w > c->width) || (dy + h > c->height)) {
                av_log(avctx, AV_LOG_ERROR,
                       "Incorrect frame size: %ix%i+%ix%i of %ix%i\n",
                       w, h, dx, dy, c->width, c->height);
                return AVERROR_INVALIDDATA;
            }
            if (size_left < w * h * c->bpp2) {
                av_log(avctx, AV_LOG_ERROR,
                       "Premature end of data! (need %i got %i)\n",
                       w * h * c->bpp2, size_left);
                return AVERROR_INVALIDDATA;
            }
            paint_raw(outptr, w, h, gb, c->bpp2, c->bigendian,
                      c->pic->linesize[0]);
            break;
        case 0x00000005: // HexTile encoded rectangle
            if ((dx + w > c->width) || (dy + h > c->height)) {
                av_log(avctx, AV_LOG_ERROR,
                       "Incorrect frame size: %ix%i+%ix%i of %ix%i\n",
                       w, h, dx, dy, c->width, c->height);
                return AVERROR_INVALIDDATA;
            }
            res = decode_hextile(c, outptr, gb, w, h, c->pic->linesize[0]);
            if (res < 0)
                return res;
            break;
        default:
            av_log(avctx, AV_LOG_ERROR, "Unsupported block type 0x%08X\n", enc);
            chunks = 0; // leave chunks decoding loop
        }
    }
    if (c->screendta) {
        int i;
        // save screen data before painting cursor
        w = c->cur_w;
        if (c->width < c->cur_x + w)
            w = c->width - c->cur_x;
        h = c->cur_h;
        if (c->height < c->cur_y + h)
            h = c->height - c->cur_y;
        dx = c->cur_x;
        if (dx < 0) {
            w += dx;
            dx = 0;
        }
        dy = c->cur_y;
        if (dy < 0) {
            h += dy;
            dy = 0;
        }
        if ((w > 0) && (h > 0)) {
            outptr = c->pic->data[0] + dx * c->bpp2 + dy * c->pic->linesize[0];
            for (i = 0; i < h; i++) {
                memcpy(c->screendta + i * c->cur_w * c->bpp2, outptr,
                       w * c->bpp2);
                outptr += c->pic->linesize[0];
            }
            outptr = c->pic->data[0];
            put_cursor(outptr, c->pic->linesize[0], c, c->cur_x, c->cur_y);
        }
    }
    *got_frame = 1;
    if ((ret = av_frame_ref(data, c->pic)) < 0)
        return ret;

    /
    return buf_size;
}
