static int read_access_unit(AVCodecContext *avctx, void* data, int *data_size,
                            AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    MLPDecodeContext *m = avctx->priv_data;
    GetBitContext gb;
    unsigned int length, substr;
    unsigned int substream_start;
    unsigned int header_size = 4;
    unsigned int substr_header_size = 0;
    uint8_t substream_parity_present[MAX_SUBSTREAMS];
    uint16_t substream_data_len[MAX_SUBSTREAMS];
    uint8_t parity_bits;

    if (buf_size < 4)
        return 0;

    length = (AV_RB16(buf) & 0xfff) * 2;

    if (length > buf_size)
        return -1;

    init_get_bits(&gb, (buf + 4), (length - 4) * 8);

    m->is_major_sync_unit = 0;
    if (show_bits_long(&gb, 31) == (0xf8726fba >> 1)) {
        if (read_major_sync(m, &gb) < 0)
            goto error;
        m->is_major_sync_unit = 1;
        header_size += 28;
    }

    if (!m->params_valid) {
        av_log(m->avctx, AV_LOG_WARNING,
               "Stream parameters not seen; skipping frame.\n");
        *data_size = 0;
        return length;
    }

    substream_start = 0;

    for (substr = 0; substr < m->num_substreams; substr++) {
        int extraword_present, checkdata_present, end, nonrestart_substr;

        extraword_present = get_bits1(&gb);
        nonrestart_substr = get_bits1(&gb);
        checkdata_present = get_bits1(&gb);
        skip_bits1(&gb);

        end = get_bits(&gb, 12) * 2;

        substr_header_size += 2;

        if (extraword_present) {
            if (m->avctx->codec_id == CODEC_ID_MLP) {
                av_log(m->avctx, AV_LOG_ERROR, "There must be no extraword for MLP.\n");
                goto error;
            }
            skip_bits(&gb, 16);
            substr_header_size += 2;
        }

        if (!(nonrestart_substr ^ m->is_major_sync_unit)) {
            av_log(m->avctx, AV_LOG_ERROR, "Invalid nonrestart_substr.\n");
            goto error;
        }

        if (end + header_size + substr_header_size > length) {
            av_log(m->avctx, AV_LOG_ERROR,
                   "Indicated length of substream %d data goes off end of "
                   "packet.\n", substr);
            end = length - header_size - substr_header_size;
        }

        if (end < substream_start) {
            av_log(avctx, AV_LOG_ERROR,
                   "Indicated end offset of substream %d data "
                   "is smaller than calculated start offset.\n",
                   substr);
            goto error;
        }

        if (substr > m->max_decoded_substream)
            continue;

        substream_parity_present[substr] = checkdata_present;
        substream_data_len[substr] = end - substream_start;
        substream_start = end;
    }

    parity_bits  = ff_mlp_calculate_parity(buf, 4);
    parity_bits ^= ff_mlp_calculate_parity(buf + header_size, substr_header_size);

    if ((((parity_bits >> 4) ^ parity_bits) & 0xF) != 0xF) {
        av_log(avctx, AV_LOG_ERROR, "Parity check failed.\n");
        goto error;
    }

    buf += header_size + substr_header_size;

    for (substr = 0; substr <= m->max_decoded_substream; substr++) {
        SubStream *s = &m->substream[substr];
        init_get_bits(&gb, buf, substream_data_len[substr] * 8);

        m->matrix_changed = 0;
        memset(m->filter_changed, 0, sizeof(m->filter_changed));

        s->blockpos = 0;
        do {
            if (get_bits1(&gb)) {
                if (get_bits1(&gb)) {
                    /
                    if (read_restart_header(m, &gb, buf, substr) < 0)
                        goto next_substr;
                    s->restart_seen = 1;
                }

                if (!s->restart_seen)
                    goto next_substr;
                if (read_decoding_params(m, &gb, substr) < 0)
                    goto next_substr;
            }

            if (!s->restart_seen)
                goto next_substr;

            if (read_block_data(m, &gb, substr) < 0)
                return -1;

            if (get_bits_count(&gb) >= substream_data_len[substr] * 8)
                goto substream_length_mismatch;

        } while (!get_bits1(&gb));

        skip_bits(&gb, (-get_bits_count(&gb)) & 15);

        if (substream_data_len[substr] * 8 - get_bits_count(&gb) >= 32) {
            int shorten_by;

            if (get_bits(&gb, 16) != 0xD234)
                return -1;

            shorten_by = get_bits(&gb, 16);
            if      (m->avctx->codec_id == CODEC_ID_TRUEHD && shorten_by  & 0x2000)
                s->blockpos -= FFMIN(shorten_by & 0x1FFF, s->blockpos);
            else if (m->avctx->codec_id == CODEC_ID_MLP    && shorten_by != 0xD234)
                return -1;

            if (substr == m->max_decoded_substream)
                av_log(m->avctx, AV_LOG_INFO, "End of stream indicated.\n");
        }

        if (substream_parity_present[substr]) {
            uint8_t parity, checksum;

            if (substream_data_len[substr] * 8 - get_bits_count(&gb) != 16)
                goto substream_length_mismatch;

            parity   = ff_mlp_calculate_parity(buf, substream_data_len[substr] - 2);
            checksum = ff_mlp_checksum8       (buf, substream_data_len[substr] - 2);

            if ((get_bits(&gb, 8) ^ parity) != 0xa9    )
                av_log(m->avctx, AV_LOG_ERROR, "Substream %d parity check failed.\n", substr);
            if ( get_bits(&gb, 8)           != checksum)
                av_log(m->avctx, AV_LOG_ERROR, "Substream %d checksum failed.\n"    , substr);
        }

        if (substream_data_len[substr] * 8 != get_bits_count(&gb))
            goto substream_length_mismatch;

next_substr:
        if (!s->restart_seen)
            av_log(m->avctx, AV_LOG_ERROR,
                   "No restart header present in substream %d.\n", substr);

        buf += substream_data_len[substr];
    }

    rematrix_channels(m, m->max_decoded_substream);

    if (output_data(m, m->max_decoded_substream, data, data_size) < 0)
        return -1;

    return length;

substream_length_mismatch:
    av_log(m->avctx, AV_LOG_ERROR, "substream %d length mismatch\n", substr);
    return -1;

error:
    m->params_valid = 0;
    return -1;
}
