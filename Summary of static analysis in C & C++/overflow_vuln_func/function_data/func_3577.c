static void gmc_motion(MpegEncContext *s,
                       uint8_t *dest_y, uint8_t *dest_cb, uint8_t *dest_cr,
                       uint8_t **ref_picture)
{
    uint8_t *ptr;
    int linesize, uvlinesize;
    const int a = s->sprite_warping_accuracy;
    int ox, oy;

    linesize   = s->linesize;
    uvlinesize = s->uvlinesize;

    ptr = ref_picture[0];

    ox = s->sprite_offset[0][0] + s->sprite_delta[0][0] * s->mb_x * 16 +
         s->sprite_delta[0][1] * s->mb_y * 16;
    oy = s->sprite_offset[0][1] + s->sprite_delta[1][0] * s->mb_x * 16 +
         s->sprite_delta[1][1] * s->mb_y * 16;

    s->mdsp.gmc(dest_y, ptr, linesize, 16,
                ox, oy,
                s->sprite_delta[0][0], s->sprite_delta[0][1],
                s->sprite_delta[1][0], s->sprite_delta[1][1],
                a + 1, (1 << (2 * a + 1)) - s->no_rounding,
                s->h_edge_pos, s->v_edge_pos);
    s->mdsp.gmc(dest_y + 8, ptr, linesize, 16,
                ox + s->sprite_delta[0][0] * 8,
                oy + s->sprite_delta[1][0] * 8,
                s->sprite_delta[0][0], s->sprite_delta[0][1],
                s->sprite_delta[1][0], s->sprite_delta[1][1],
                a + 1, (1 << (2 * a + 1)) - s->no_rounding,
                s->h_edge_pos, s->v_edge_pos);

    if (CONFIG_GRAY && s->flags & CODEC_FLAG_GRAY)
        return;

    ox = s->sprite_offset[1][0] + s->sprite_delta[0][0] * s->mb_x * 8 +
         s->sprite_delta[0][1] * s->mb_y * 8;
    oy = s->sprite_offset[1][1] + s->sprite_delta[1][0] * s->mb_x * 8 +
         s->sprite_delta[1][1] * s->mb_y * 8;

    ptr = ref_picture[1];
    s->mdsp.gmc(dest_cb, ptr, uvlinesize, 8,
                ox, oy,
                s->sprite_delta[0][0], s->sprite_delta[0][1],
                s->sprite_delta[1][0], s->sprite_delta[1][1],
                a + 1, (1 << (2 * a + 1)) - s->no_rounding,
                s->h_edge_pos >> 1, s->v_edge_pos >> 1);

    ptr = ref_picture[2];
    s->mdsp.gmc(dest_cr, ptr, uvlinesize, 8,
                ox, oy,
                s->sprite_delta[0][0], s->sprite_delta[0][1],
                s->sprite_delta[1][0], s->sprite_delta[1][1],
                a + 1, (1 << (2 * a + 1)) - s->no_rounding,
                s->h_edge_pos >> 1, s->v_edge_pos >> 1);
}
