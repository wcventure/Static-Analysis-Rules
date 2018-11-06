static void encode_frame(MpegAudioContext *s,
                         unsigned char bit_alloc[MPA_MAX_CHANNELS][SBLIMIT],
                         int padding)
{
    int i, j, k, l, bit_alloc_bits, b, ch;
    unsigned char *sf;
    int q[3];
    PutBitContext *p = &s->pb;

    /

    put_bits(p, 12, 0xfff);
    put_bits(p, 1, 1 - s->lsf); /
    put_bits(p, 2, 4-2);  /
    put_bits(p, 1, 1); /
    put_bits(p, 4, s->bitrate_index);
    put_bits(p, 2, s->freq_index);
    put_bits(p, 1, s->do_padding); /
    put_bits(p, 1, 0);             /
    put_bits(p, 2, s->nb_channels == 2 ? MPA_STEREO : MPA_MONO);
    put_bits(p, 2, 0); /
    put_bits(p, 1, 0); /
    put_bits(p, 1, 1); /
    put_bits(p, 2, 0); /

    /
    j = 0;
    for(i=0;i<s->sblimit;i++) {
        bit_alloc_bits = s->alloc_table[j];
        for(ch=0;ch<s->nb_channels;ch++) {
            put_bits(p, bit_alloc_bits, bit_alloc[ch][i]);
        }
        j += 1 << bit_alloc_bits;
    }

    /
    for(i=0;i<s->sblimit;i++) {
        for(ch=0;ch<s->nb_channels;ch++) {
            if (bit_alloc[ch][i])
                put_bits(p, 2, s->scale_code[ch][i]);
        }
    }

    /
    for(i=0;i<s->sblimit;i++) {
        for(ch=0;ch<s->nb_channels;ch++) {
            if (bit_alloc[ch][i]) {
                sf = &s->scale_factors[ch][i][0];
                switch(s->scale_code[ch][i]) {
                case 0:
                    put_bits(p, 6, sf[0]);
                    put_bits(p, 6, sf[1]);
                    put_bits(p, 6, sf[2]);
                    break;
                case 3:
                case 1:
                    put_bits(p, 6, sf[0]);
                    put_bits(p, 6, sf[2]);
                    break;
                case 2:
                    put_bits(p, 6, sf[0]);
                    break;
                }
            }
        }
    }

    /

    for(k=0;k<3;k++) {
        for(l=0;l<12;l+=3) {
            j = 0;
            for(i=0;i<s->sblimit;i++) {
                bit_alloc_bits = s->alloc_table[j];
                for(ch=0;ch<s->nb_channels;ch++) {
                    b = bit_alloc[ch][i];
                    if (b) {
                        int qindex, steps, m, sample, bits;
                        /
                        qindex = s->alloc_table[j+b];
                        steps = ff_mpa_quant_steps[qindex];
                        for(m=0;m<3;m++) {
                            sample = s->sb_samples[ch][k][l + m][i];
                            /
#if USE_FLOATS
                            {
                                float a;
                                a = (float)sample * s->scale_factor_inv_table[s->scale_factors[ch][i][k]];
                                q[m] = (int)((a + 1.0) * steps * 0.5);
                            }
#else
                            {
                                int q1, e, shift, mult;
                                e = s->scale_factors[ch][i][k];
                                shift = s->scale_factor_shift[e];
                                mult = s->scale_factor_mult[e];

                                /
                                if (shift < 0)
                                    q1 = sample << (-shift);
                                else
                                    q1 = sample >> shift;
                                q1 = (q1 * mult) >> P;
                                q[m] = ((q1 + (1 << P)) * steps) >> (P + 1);
                                if (q[m] < 0)
                                    q[m] = 0;
                            }
#endif
                            if (q[m] >= steps)
                                q[m] = steps - 1;
                            av_assert2(q[m] >= 0 && q[m] < steps);
                        }
                        bits = ff_mpa_quant_bits[qindex];
                        if (bits < 0) {
                            /
                            put_bits(p, -bits,
                                     q[0] + steps * (q[1] + steps * q[2]));
                        } else {
                            put_bits(p, bits, q[0]);
                            put_bits(p, bits, q[1]);
                            put_bits(p, bits, q[2]);
                        }
                    }
                }
                /
                j += 1 << bit_alloc_bits;
            }
        }
    }

    /
    for(i=0;i<padding;i++)
        put_bits(p, 1, 0);

    /
    flush_put_bits(p);
}
