static void vp56_mc(VP56Context *s, int b, int plane, uint8_t *src,
                    int stride, int x, int y)
{
    uint8_t *dst = s->frames[VP56_FRAME_CURRENT]->data[plane] + s->block_offset[b];
    uint8_t *src_block;
    int src_offset;
    int overlap_offset = 0;
    int mask = s->vp56_coord_div[b] - 1;
    int deblock_filtering = s->deblock_filtering;
    int dx;
    int dy;

    if (s->avctx->skip_loop_filter >= AVDISCARD_ALL ||
        (s->avctx->skip_loop_filter >= AVDISCARD_NONKEY
         && !s->frames[VP56_FRAME_CURRENT]->key_frame))
        deblock_filtering = 0;

    dx = s->mv[b].x / s->vp56_coord_div[b];
    dy = s->mv[b].y / s->vp56_coord_div[b];

    if (b >= 4) {
        x /= 2;
        y /= 2;
    }
    x += dx - 2;
    y += dy - 2;

    if (x<0 || x+12>=s->plane_width[plane] ||
        y<0 || y+12>=s->plane_height[plane]) {
        s->vdsp.emulated_edge_mc(s->edge_emu_buffer,
                            src + s->block_offset[b] + (dy-2)*stride + (dx-2),
                            stride, 12, 12, x, y,
                            s->plane_width[plane],
                            s->plane_height[plane]);
        src_block = s->edge_emu_buffer;
        src_offset = 2 + 2*stride;
    } else if (deblock_filtering) {
        /
        /
        s->hdsp.put_pixels_tab[0][0](s->edge_emu_buffer,
                                     src + s->block_offset[b] + (dy-2)*stride + (dx-2),
                                     stride, 12);
        src_block = s->edge_emu_buffer;
        src_offset = 2 + 2*stride;
    } else {
        src_block = src;
        src_offset = s->block_offset[b] + dy*stride + dx;
    }

    if (deblock_filtering)
        vp56_deblock_filter(s, src_block, stride, dx&7, dy&7);

    if (s->mv[b].x & mask)
        overlap_offset += (s->mv[b].x > 0) ? 1 : -1;
    if (s->mv[b].y & mask)
        overlap_offset += (s->mv[b].y > 0) ? stride : -stride;

    if (overlap_offset) {
        if (s->filter)
            s->filter(s, dst, src_block, src_offset, src_offset+overlap_offset,
                      stride, s->mv[b], mask, s->filter_selection, b<4);
        else
            s->vp3dsp.put_no_rnd_pixels_l2(dst, src_block+src_offset,
                                           src_block+src_offset+overlap_offset,
                                           stride, 8);
    } else {
        s->hdsp.put_pixels_tab[1][0](dst, src_block+src_offset, stride, 8);
    }
}
