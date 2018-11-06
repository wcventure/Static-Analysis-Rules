static av_always_inline void dnxhd_decode_dct_block(const DNXHDContext *ctx,
                                                    RowContext *row,
                                                    int n,
                                                    int index_bits,
                                                    int level_bias,
                                                    int level_shift)
{
    int i, j, index1, index2, len, flags;
    int level, component, sign;
    const int *scale;
    const uint8_t *weight_matrix;
    const uint8_t *ac_level = ctx->cid_table->ac_level;
    const uint8_t *ac_flags = ctx->cid_table->ac_flags;
    int16_t *block = row->blocks[n];
    const int eob_index     = ctx->cid_table->eob_index;
    OPEN_READER(bs, &row->gb);

    ctx->bdsp.clear_block(block);

    if (!ctx->is_444) {
        if (n & 2) {
            component     = 1 + (n & 1);
            scale = row->chroma_scale;
            weight_matrix = ctx->cid_table->chroma_weight;
        } else {
            component     = 0;
            scale = row->luma_scale;
            weight_matrix = ctx->cid_table->luma_weight;
        }
    } else {
        component = (n >> 1) % 3;
        if (component) {
            scale = row->chroma_scale;
            weight_matrix = ctx->cid_table->chroma_weight;
        } else {
            scale = row->luma_scale;
            weight_matrix = ctx->cid_table->luma_weight;
        }
    }

    UPDATE_CACHE(bs, &row->gb);
    GET_VLC(len, bs, &row->gb, ctx->dc_vlc.table, DNXHD_DC_VLC_BITS, 1);
    if (len) {
        level = GET_CACHE(bs, &row->gb);
        LAST_SKIP_BITS(bs, &row->gb, len);
        sign  = ~level >> 31;
        level = (NEG_USR32(sign ^ level, len) ^ sign) - sign;
        row->last_dc[component] += level;
    }
    block[0] = row->last_dc[component];

    i = 0;

    UPDATE_CACHE(bs, &row->gb);
    GET_VLC(index1, bs, &row->gb, ctx->ac_vlc.table,
            DNXHD_VLC_BITS, 2);

    while (index1 != eob_index) {
        level = ac_level[index1];
        flags = ac_flags[index1];

        sign = SHOW_SBITS(bs, &row->gb, 1);
        SKIP_BITS(bs, &row->gb, 1);

        if (flags & 1) {
            level += SHOW_UBITS(bs, &row->gb, index_bits) << 7;
            SKIP_BITS(bs, &row->gb, index_bits);
        }

        if (flags & 2) {
            UPDATE_CACHE(bs, &row->gb);
            GET_VLC(index2, bs, &row->gb, ctx->run_vlc.table,
                    DNXHD_VLC_BITS, 2);
            i += ctx->cid_table->run[index2];
        }

        if (++i > 63) {
            av_log(ctx->avctx, AV_LOG_ERROR, "ac tex damaged %d, %d\n", n, i);
            break;
        }

        j     = ctx->scantable.permutated[i];
        level *= scale[i];
        if (level_bias < 32 || weight_matrix[i] != level_bias)
            level += level_bias;
        level >>= level_shift;

        block[j] = (level ^ sign) - sign;

        UPDATE_CACHE(bs, &row->gb);
        GET_VLC(index1, bs, &row->gb, ctx->ac_vlc.table,
                DNXHD_VLC_BITS, 2);
    }

    CLOSE_READER(bs, &row->gb);
}
