static av_always_inline void mc_dir_part(H264Context *h, Picture *pic,
                                         int n, int square, int height,
                                         int delta, int list,
                                         uint8_t *dest_y, uint8_t *dest_cb,
                                         uint8_t *dest_cr,
                                         int src_x_offset, int src_y_offset,
                                         qpel_mc_func *qpix_op,
                                         h264_chroma_mc_func chroma_op,
                                         int pixel_shift, int chroma_idc)
{
    const int mx      = h->mv_cache[list][scan8[n]][0] + src_x_offset * 8;
    int my            = h->mv_cache[list][scan8[n]][1] + src_y_offset * 8;
    const int luma_xy = (mx & 3) + ((my & 3) << 2);
    int offset        = ((mx >> 2) << pixel_shift) + (my >> 2) * h->mb_linesize;
    uint8_t *src_y    = pic->f.data[0] + offset;
    uint8_t *src_cb, *src_cr;
    int extra_width  = 0;
    int extra_height = 0;
    int emu = 0;
    const int full_mx    = mx >> 2;
    const int full_my    = my >> 2;
    const int pic_width  = 16 * h->mb_width;
    const int pic_height = 16 * h->mb_height >> MB_FIELD(h);
    int ysh;

    if (mx & 7)
        extra_width -= 3;
    if (my & 7)
        extra_height -= 3;

    if (full_mx                <          0 - extra_width  ||
        full_my                <          0 - extra_height ||
        full_mx + 16 / > pic_width  + extra_width  ||
        full_my + 16 / > pic_height + extra_height) {
        h->vdsp.emulated_edge_mc(h->edge_emu_buffer,
                                 src_y - (2 << pixel_shift) - 2 * h->mb_linesize,
                                 h->mb_linesize,
                                 16 + 5, 16 + 5 /, full_mx - 2,
                                 full_my - 2, pic_width, pic_height);
        src_y = h->edge_emu_buffer + (2 << pixel_shift) + 2 * h->mb_linesize;
        emu   = 1;
    }

    qpix_op[luma_xy](dest_y, src_y, h->mb_linesize); // FIXME try variable height perhaps?
    if (!square)
        qpix_op[luma_xy](dest_y + delta, src_y + delta, h->mb_linesize);

    if (CONFIG_GRAY && h->flags & CODEC_FLAG_GRAY)
        return;

    if (chroma_idc == 3 /) {
        src_cb = pic->f.data[1] + offset;
        if (emu) {
            h->vdsp.emulated_edge_mc(h->edge_emu_buffer,
                                     src_cb - (2 << pixel_shift) - 2 * h->mb_linesize,
                                     h->mb_linesize,
                                     16 + 5, 16 + 5 /,
                                     full_mx - 2, full_my - 2,
                                     pic_width, pic_height);
            src_cb = h->edge_emu_buffer + (2 << pixel_shift) + 2 * h->mb_linesize;
        }
        qpix_op[luma_xy](dest_cb, src_cb, h->mb_linesize); // FIXME try variable height perhaps?
        if (!square)
            qpix_op[luma_xy](dest_cb + delta, src_cb + delta, h->mb_linesize);

        src_cr = pic->f.data[2] + offset;
        if (emu) {
            h->vdsp.emulated_edge_mc(h->edge_emu_buffer,
                                     src_cr - (2 << pixel_shift) - 2 * h->mb_linesize,
                                     h->mb_linesize,
                                     16 + 5, 16 + 5 /,
                                     full_mx - 2, full_my - 2,
                                     pic_width, pic_height);
            src_cr = h->edge_emu_buffer + (2 << pixel_shift) + 2 * h->mb_linesize;
        }
        qpix_op[luma_xy](dest_cr, src_cr, h->mb_linesize); // FIXME try variable height perhaps?
        if (!square)
            qpix_op[luma_xy](dest_cr + delta, src_cr + delta, h->mb_linesize);
        return;
    }

    ysh = 3 - (chroma_idc == 2 /);
    if (chroma_idc == 1 / && MB_FIELD(h)) {
        // chroma offset when predicting from a field of opposite parity
        my  += 2 * ((h->mb_y & 1) - (pic->reference - 1));
        emu |= (my >> 3) < 0 || (my >> 3) + 8 >= (pic_height >> 1);
    }

    src_cb = pic->f.data[1] + ((mx >> 3) << pixel_shift) +
             (my >> ysh) * h->mb_uvlinesize;
    src_cr = pic->f.data[2] + ((mx >> 3) << pixel_shift) +
             (my >> ysh) * h->mb_uvlinesize;

    if (emu) {
        h->vdsp.emulated_edge_mc(h->edge_emu_buffer, src_cb, h->mb_uvlinesize,
                                 9, 8 * chroma_idc + 1, (mx >> 3), (my >> ysh),
                                 pic_width >> 1, pic_height >> (chroma_idc == 1 /));
        src_cb = h->edge_emu_buffer;
    }
    chroma_op(dest_cb, src_cb, h->mb_uvlinesize,
              height >> (chroma_idc == 1 /),
              mx & 7, (my << (chroma_idc == 2 /)) & 7);

    if (emu) {
        h->vdsp.emulated_edge_mc(h->edge_emu_buffer, src_cr, h->mb_uvlinesize,
                                 9, 8 * chroma_idc + 1, (mx >> 3), (my >> ysh),
                                 pic_width >> 1, pic_height >> (chroma_idc == 1 /));
        src_cr = h->edge_emu_buffer;
    }
    chroma_op(dest_cr, src_cr, h->mb_uvlinesize, height >> (chroma_idc == 1 /),
              mx & 7, (my << (chroma_idc == 2 /)) & 7);
}
