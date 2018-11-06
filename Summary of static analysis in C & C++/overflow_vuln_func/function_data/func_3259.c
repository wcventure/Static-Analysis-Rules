static int decode_init(AVCodecContext * avctx)
{
    MPADecodeContext *s = avctx->priv_data;
    static int init;
    int i, j, k;

    if(!init) {
        /
        for(i=0;i<64;i++) {
            int shift, mod;
            /
            shift = (i / 3) - 1;
            mod = i % 3;
#if FRAC_BITS <= 15
            if (shift > 31)
                shift = 31;
#endif
            scale_factor_modshift[i] = mod | (shift << 2);
        }

        /
        for(i=0;i<15;i++) {
            int n, norm;
            n = i + 2;
            norm = ((INT64_C(1) << n) * FRAC_ONE) / ((1 << n) - 1);
            scale_factor_mult[i][0] = MULL(FIXR(1.0), norm);
            scale_factor_mult[i][1] = MULL(FIXR(0.7937005259), norm);
            scale_factor_mult[i][2] = MULL(FIXR(0.6299605249), norm);
            dprintf("%d: norm=%x s=%x %x %x\n",
                    i, norm, 
                    scale_factor_mult[i][0],
                    scale_factor_mult[i][1],
                    scale_factor_mult[i][2]);
        }
        
        /
        /
        for(i=0;i<257;i++) {
            int v;
            v = mpa_enwindow[i];
#if WFRAC_BITS < 16
            v = (v + (1 << (16 - WFRAC_BITS - 1))) >> (16 - WFRAC_BITS);
#endif
            window[i] = v;
            if ((i & 63) != 0)
                v = -v;
            if (i != 0)
                window[512 - i] = v;
        }
        
        /
        huff_code_table[0] = NULL;
        for(i=1;i<16;i++) {
            const HuffTable *h = &mpa_huff_tables[i];
            int xsize, n, x, y;
            UINT8 *code_table;

            xsize = h->xsize;
            n = xsize * xsize;
            /
            init_vlc(&huff_vlc[i], 8, n, 
                     h->bits, 1, 1, h->codes, 2, 2);
            
            code_table = av_mallocz(n);
            j = 0;
            for(x=0;x<xsize;x++) {
                for(y=0;y<xsize;y++)
                    code_table[j++] = (x << 4) | y;
            }
            huff_code_table[i] = code_table;
        }
        for(i=0;i<2;i++) {
            init_vlc(&huff_quad_vlc[i], i == 0 ? 7 : 4, 16, 
                     mpa_quad_bits[i], 1, 1, mpa_quad_codes[i], 1, 1);
        }

        for(i=0;i<9;i++) {
            k = 0;
            for(j=0;j<22;j++) {
                band_index_long[i][j] = k;
                k += band_size_long[i][j];
            }
            band_index_long[i][22] = k;
        }

        /
        table_4_3_exp = av_mallocz(TABLE_4_3_SIZE * 
                                   sizeof(table_4_3_exp[0]));
        if (!table_4_3_exp)
            return -1;
        table_4_3_value = av_mallocz(TABLE_4_3_SIZE * 
                                     sizeof(table_4_3_value[0]));
        if (!table_4_3_value) {
            av_free(table_4_3_exp);
            return -1;
        }
        
        int_pow_init();
        for(i=1;i<TABLE_4_3_SIZE;i++) {
            int e, m;
            m = int_pow(i, &e);
#if 0
            /
            {
                double f, fm;
                int e1, m1;
                f = pow((double)i, 4.0 / 3.0);
                fm = frexp(f, &e1);
                m1 = FIXR(2 * fm);
#if FRAC_BITS <= 15
                if ((unsigned short)m1 != m1) {
                    m1 = m1 >> 1;
                    e1++;
                }
#endif
                e1--;
                if (m != m1 || e != e1) {
                    printf("%4d: m=%x m1=%x e=%d e1=%d\n",
                           i, m, m1, e, e1);
                }
            }
#endif
            /
            table_4_3_value[i] = m;
            table_4_3_exp[i] = e;
        }
        
        for(i=0;i<7;i++) {
            float f;
            int v;
            if (i != 6) {
                f = tan((double)i * M_PI / 12.0);
                v = FIXR(f / (1.0 + f));
            } else {
                v = FIXR(1.0);
            }
            is_table[0][i] = v;
            is_table[1][6 - i] = v;
        }
        /
        for(i=7;i<16;i++)
            is_table[0][i] = is_table[1][i] = 0.0;

        for(i=0;i<16;i++) {
            double f;
            int e, k;

            for(j=0;j<2;j++) {
                e = -(j + 1) * ((i + 1) >> 1);
                f = pow(2.0, e / 4.0);
                k = i & 1;
                is_table_lsf[j][k ^ 1][i] = FIXR(f);
                is_table_lsf[j][k][i] = FIXR(1.0);
                dprintf("is_table_lsf %d %d: %x %x\n", 
                        i, j, is_table_lsf[j][0][i], is_table_lsf[j][1][i]);
            }
        }

        for(i=0;i<8;i++) {
            float ci, cs, ca;
            ci = ci_table[i];
            cs = 1.0 / sqrt(1.0 + ci * ci);
            ca = cs * ci;
            csa_table[i][0] = FIX(cs);
            csa_table[i][1] = FIX(ca);
        }

        /
        for(i=0;i<36;i++) {
            int v;
            v = FIXR(sin(M_PI * (i + 0.5) / 36.0));
            mdct_win[0][i] = v;
            mdct_win[1][i] = v;
            mdct_win[3][i] = v;
        }
        for(i=0;i<6;i++) {
            mdct_win[1][18 + i] = FIXR(1.0);
            mdct_win[1][24 + i] = FIXR(sin(M_PI * ((i + 6) + 0.5) / 12.0));
            mdct_win[1][30 + i] = FIXR(0.0);

            mdct_win[3][i] = FIXR(0.0);
            mdct_win[3][6 + i] = FIXR(sin(M_PI * (i + 0.5) / 12.0));
            mdct_win[3][12 + i] = FIXR(1.0);
        }

        for(i=0;i<12;i++)
            mdct_win[2][i] = FIXR(sin(M_PI * (i + 0.5) / 12.0));
        
        /
        for(j=0;j<4;j++) {
            for(i=0;i<36;i+=2) {
                mdct_win[j + 4][i] = mdct_win[j][i];
                mdct_win[j + 4][i + 1] = -mdct_win[j][i + 1];
            }
        }

#if defined(DEBUG)
        for(j=0;j<8;j++) {
            printf("win%d=\n", j);
            for(i=0;i<36;i++)
                printf("%f, ", (double)mdct_win[j][i] / FRAC_ONE);
            printf("\n");
        }
#endif
        init = 1;
    }

    s->inbuf_index = 0;
    s->inbuf = &s->inbuf1[s->inbuf_index][BACKSTEP_SIZE];
    s->inbuf_ptr = s->inbuf;
#ifdef DEBUG
    s->frame_count = 0;
#endif
    return 0;
}
