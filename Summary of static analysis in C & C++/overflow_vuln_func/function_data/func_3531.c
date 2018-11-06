static int smka_decode_frame(AVCodecContext *avctx, void *data, int *data_size, uint8_t *buf, int buf_size)
{
    GetBitContext gb;
    HuffContext h[4];
    VLC vlc[4];
    int16_t *samples = data;
    int val;
    int i, res;
    int unp_size;
    int bits, stereo;
    int pred[2] = {0, 0};

    unp_size = AV_RL32(buf);

    init_get_bits(&gb, buf + 4, (buf_size - 4) * 8);

    if(!get_bits1(&gb)){
        av_log(avctx, AV_LOG_INFO, "Sound: no data\n");
        *data_size = 0;
        return 1;
    }
    stereo = get_bits1(&gb);
    bits = get_bits1(&gb);
    if ((unp_size << !bits) > *data_size) {
        av_log(avctx, AV_LOG_ERROR, "Frame is too large to fit in buffer\n");
        return -1;
    }

    memset(vlc, 0, sizeof(VLC) * 4);
    memset(h, 0, sizeof(HuffContext) * 4);
    // Initialize
    for(i = 0; i < (1 << (bits + stereo)); i++) {
        h[i].length = 256;
        h[i].maxlength = 0;
        h[i].current = 0;
        h[i].bits = av_mallocz(256 * 4);
        h[i].lengths = av_mallocz(256 * sizeof(int));
        h[i].values = av_mallocz(256 * sizeof(int));
        skip_bits1(&gb);
        smacker_decode_tree(&gb, &h[i], 0, 0);
        skip_bits1(&gb);
        if(h[i].current > 1) {
            res = init_vlc(&vlc[i], SMKTREE_BITS, h[i].length,
                    h[i].lengths, sizeof(int), sizeof(int),
                    h[i].bits, sizeof(uint32_t), sizeof(uint32_t), INIT_VLC_LE);
            if(res < 0) {
                av_log(avctx, AV_LOG_ERROR, "Cannot build VLC table\n");
                return -1;
            }
        }
    }
    if(bits) { //decode 16-bit data
        for(i = stereo; i >= 0; i--)
            pred[i] = bswap_16(get_bits(&gb, 16));
        for(i = 0; i < stereo; i++)
            *samples++ = pred[i];
        for(i = 0; i < unp_size / 2; i++) {
            if(i & stereo) {
                if(vlc[2].table)
                    res = get_vlc2(&gb, vlc[2].table, SMKTREE_BITS, 3);
                else
                    res = 0;
                val  = h[2].values[res];
                if(vlc[3].table)
                    res = get_vlc2(&gb, vlc[3].table, SMKTREE_BITS, 3);
                else
                    res = 0;
                val |= h[3].values[res] << 8;
                pred[1] += (int16_t)val;
                *samples++ = pred[1];
            } else {
                if(vlc[0].table)
                    res = get_vlc2(&gb, vlc[0].table, SMKTREE_BITS, 3);
                else
                    res = 0;
                val  = h[0].values[res];
                if(vlc[1].table)
                    res = get_vlc2(&gb, vlc[1].table, SMKTREE_BITS, 3);
                else
                    res = 0;
                val |= h[1].values[res] << 8;
                pred[0] += val;
                *samples++ = pred[0];
            }
        }
    } else { //8-bit data
        for(i = stereo; i >= 0; i--)
            pred[i] = get_bits(&gb, 8);
        for(i = 0; i < stereo; i++)
            *samples++ = (pred[i] - 0x80) << 8;
        for(i = 0; i < unp_size; i++) {
            if(i & stereo){
                if(vlc[1].table)
                    res = get_vlc2(&gb, vlc[1].table, SMKTREE_BITS, 3);
                else
                    res = 0;
                pred[1] += (int8_t)h[1].values[res];
                *samples++ = (pred[1] - 0x80) << 8;
            } else {
                if(vlc[0].table)
                    res = get_vlc2(&gb, vlc[0].table, SMKTREE_BITS, 3);
                else
                    res = 0;
                pred[0] += (int8_t)h[0].values[res];
                *samples++ = (pred[0] - 0x80) << 8;
            }
        }
        unp_size *= 2;
    }

    for(i = 0; i < 4; i++) {
        if(vlc[i].table)
            free_vlc(&vlc[i]);
        if(h[i].bits)
            av_free(h[i].bits);
        if(h[i].lengths)
            av_free(h[i].lengths);
        if(h[i].values)
            av_free(h[i].values);
    }

    *data_size = unp_size;
    return buf_size;
}
