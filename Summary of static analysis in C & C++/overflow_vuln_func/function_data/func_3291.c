static int mjpeg_decode_scan(MJpegDecodeContext *s, int nb_components, int Ah,
                             int Al, const uint8_t *mb_bitmask,
                             int mb_bitmask_size,
                             const AVFrame *reference)
{
    int i, mb_x, mb_y;
    uint8_t *data[MAX_COMPONENTS];
    const uint8_t *reference_data[MAX_COMPONENTS];
    int linesize[MAX_COMPONENTS];
    GetBitContext mb_bitmask_gb = {0}; // initialize to silence gcc warning
    int bytes_per_pixel = 1 + (s->bits > 8);

    if (mb_bitmask) {
        if (mb_bitmask_size != (s->mb_width * s->mb_height + 7)>>3) {
            av_log(s->avctx, AV_LOG_ERROR, "mb_bitmask_size mismatches\n");
            return AVERROR_INVALIDDATA;
        }
        init_get_bits(&mb_bitmask_gb, mb_bitmask, s->mb_width * s->mb_height);
    }

    s->restart_count = 0;

    for (i = 0; i < nb_components; i++) {
        int c   = s->comp_index[i];
        data[c] = s->picture_ptr->data[c];
        reference_data[c] = reference ? reference->data[c] : NULL;
        linesize[c] = s->linesize[c];
        s->coefs_finished[c] |= 1;
    }

    for (mb_y = 0; mb_y < s->mb_height; mb_y++) {
        for (mb_x = 0; mb_x < s->mb_width; mb_x++) {
            const int copy_mb = mb_bitmask && !get_bits1(&mb_bitmask_gb);

            if (s->restart_interval && !s->restart_count)
                s->restart_count = s->restart_interval;

            if (get_bits_left(&s->gb) < 0) {
                av_log(s->avctx, AV_LOG_ERROR, "overread %d\n",
                       -get_bits_left(&s->gb));
                return AVERROR_INVALIDDATA;
            }
            for (i = 0; i < nb_components; i++) {
                uint8_t *ptr;
                int n, h, v, x, y, c, j;
                int block_offset;
                n = s->nb_blocks[i];
                c = s->comp_index[i];
                h = s->h_scount[i];
                v = s->v_scount[i];
                x = 0;
                y = 0;
                for (j = 0; j < n; j++) {
                    block_offset = (((linesize[c] * (v * mb_y + y) * 8) +
                                     (h * mb_x + x) * 8 * bytes_per_pixel) >> s->avctx->lowres);

                    if (s->interlaced && s->bottom_field)
                        block_offset += linesize[c] >> 1;
                    ptr = data[c] + block_offset;
                    if (!s->progressive) {
                        if (copy_mb)
                            mjpeg_copy_block(s, ptr, reference_data[c] + block_offset,
                                             linesize[c], s->avctx->lowres);

                        else {
                            s->bdsp.clear_block(s->block);
                            if (decode_block(s, s->block, i,
                                             s->dc_index[i], s->ac_index[i],
                                             s->quant_matrixes[s->quant_sindex[i]]) < 0) {
                                av_log(s->avctx, AV_LOG_ERROR,
                                       "error y=%d x=%d\n", mb_y, mb_x);
                                return AVERROR_INVALIDDATA;
                            }
                            s->idsp.idct_put(ptr, linesize[c], s->block);
                            if (s->bits & 7)
                                shift_output(s, ptr, linesize[c]);
                        }
                    } else {
                        int block_idx  = s->block_stride[c] * (v * mb_y + y) +
                                         (h * mb_x + x);
                        int16_t *block = s->blocks[c][block_idx];
                        if (Ah)
                            block[0] += get_bits1(&s->gb) *
                                        s->quant_matrixes[s->quant_sindex[i]][0] << Al;
                        else if (decode_dc_progressive(s, block, i, s->dc_index[i],
                                                       s->quant_matrixes[s->quant_sindex[i]],
                                                       Al) < 0) {
                            av_log(s->avctx, AV_LOG_ERROR,
                                   "error y=%d x=%d\n", mb_y, mb_x);
                            return AVERROR_INVALIDDATA;
                        }
                    }
                    av_dlog(s->avctx, "mb: %d %d processed\n", mb_y, mb_x);
                    av_dlog(s->avctx, "%d %d %d %d %d %d %d %d \n",
                            mb_x, mb_y, x, y, c, s->bottom_field,
                            (v * mb_y + y) * 8, (h * mb_x + x) * 8);
                    if (++x == h) {
                        x = 0;
                        y++;
                    }
                }
            }

            handle_rstn(s, nb_components);
        }
    }
    return 0;
}
