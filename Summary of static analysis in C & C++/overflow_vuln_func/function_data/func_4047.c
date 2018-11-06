int MPA_encode_init(AVCodecContext *avctx)
{
    MpegAudioContext *s = avctx->priv_data;
    int freq = avctx->sample_rate;
    int bitrate = avctx->bit_rate;
    int channels = avctx->channels;
    int i, v, table;
    float a;

    if (channels > 2)
        return -1;
    bitrate = bitrate / 1000;
    s->nb_channels = channels;
    s->freq = freq;
    s->bit_rate = bitrate * 1000;
    avctx->frame_size = MPA_FRAME_SIZE;
    avctx->key_frame = 1; /

    /
    s->lsf = 0;
    for(i=0;i<3;i++) {
        if (mpa_freq_tab[i] == freq) 
            break;
        if ((mpa_freq_tab[i] / 2) == freq) {
            s->lsf = 1;
            break;
        }
    }
    if (i == 3)
        return -1;
    s->freq_index = i;

    /
    for(i=0;i<15;i++) {
        if (mpa_bitrate_tab[s->lsf][1][i] == bitrate) 
            break;
    }
    if (i == 15)
        return -1;
    s->bitrate_index = i;

    /
    
    a = (float)(bitrate * 1000 * MPA_FRAME_SIZE) / (freq * 8.0);
    s->frame_size = ((int)a) * 8;

    /
    s->frame_frac = 0;
    s->frame_frac_incr = (int)((a - floor(a)) * 65536.0);
    
    /
    table = l2_select_table(bitrate, s->nb_channels, freq, s->lsf);

    /
    s->sblimit = sblimit_table[table];
    s->alloc_table = alloc_tables[table];

#ifdef DEBUG
    printf("%d kb/s, %d Hz, frame_size=%d bits, table=%d, padincr=%x\n", 
           bitrate, freq, s->frame_size, table, s->frame_frac_incr);
#endif

    for(i=0;i<s->nb_channels;i++)
        s->samples_offset[i] = 0;

    for(i=0;i<257;i++) {
        int v;
        v = (mpa_enwindow[i] + 2) >> 2;
        filter_bank[i] = v;
        if ((i & 63) != 0)
            v = -v;
        if (i != 0)
            filter_bank[512 - i] = v;
    }

    for(i=0;i<64;i++) {
        v = (int)(pow(2.0, (3 - i) / 3.0) * (1 << 20));
        if (v <= 0)
            v = 1;
        scale_factor_table[i] = v;
#ifdef USE_FLOATS
        scale_factor_inv_table[i] = pow(2.0, -(3 - i) / 3.0) / (float)(1 << 20);
#else
#define P 15
        scale_factor_shift[i] = 21 - P - (i / 3);
        scale_factor_mult[i] = (1 << P) * pow(2.0, (i % 3) / 3.0);
#endif
    }
    for(i=0;i<128;i++) {
        v = i - 64;
        if (v <= -3)
            v = 0;
        else if (v < 0)
            v = 1;
        else if (v == 0)
            v = 2;
        else if (v < 3)
            v = 3;
        else 
            v = 4;
        scale_diff_table[i] = v;
    }

    for(i=0;i<17;i++) {
        v = quant_bits[i];
        if (v < 0) 
            v = -v;
        else
            v = v * 3;
        total_quant_bits[i] = 12 * v;
    }

    return 0;
}
