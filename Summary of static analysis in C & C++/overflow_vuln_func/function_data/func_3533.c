static int decode_init(AVCodecContext * avctx)
{
    MPADecodeContext *s = avctx->priv_data;
    static int init=0;
    int i, j, k;

#if defined(USE_HIGHPRECISION) && defined(CONFIG_AUDIO_NONSHORT)
    avctx->sample_fmt= SAMPLE_FMT_S32;
#else
    avctx->sample_fmt= SAMPLE_FMT_S16;
#endif    
    
    if(avctx->antialias_algo != FF_AA_FLOAT)
        s->compute_antialias= compute_antialias_integer;
    else
        s->compute_antialias= compute_antialias_float;

    if (!init && !avctx->parse_only) {
        /
        for(i=0;i<64;i++) {
            int shift, mod;
            /
            shift = (i / 3);
            mod = i % 3;
            scale_factor_modshift[i] = mod | (shift << 2);
        }

        /
        for(i=0;i<15;i++) {
            int n, norm;
            n = i + 2;
            norm = ((int64_t_C(1) << n) * FRAC_ONE) / ((1 << n) - 1);
            scale_factor_mult[i][0] = MULL(FIXR(1.0 * 2.0), norm);
            scale_factor_mult[i][1] = MULL(FIXR(0.7937005259 * 2.0), norm);
            scale_factor_mult[i][2] = MULL(FIXR(0.6299605249 * 2.0), norm);
            dprintf("%d: norm=%x s=%x %x %x\n",
                    i, norm, 
                    scale_factor_mult[i][0],
                    scale_factor_mult[i][1],
                    scale_factor_mult[i][2]);
        }
        
	ff_mpa_synth_init(window);
        
        /
        huff_code_table[0] = NULL;
        for(i=1;i<16;i++) {
            const HuffTable *h = &mpa_huff_tables[i];
	    int xsize, x, y;
            unsigned int n;
            uint8_t *code_table;

            xsize = h->xsize;
            n = xsize * xsize;
            /
            init_vlc(&huff_vlc[i], 8, n, 
                     h->bits, 1, 1, h->codes, 2, 2, 1);
            
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
                     mpa_quad_bits[i], 1, 1, mpa_quad_codes[i], 1, 1, 1);
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
	table_4_3_exp= av_mallocz_static(TABLE_4_3_SIZE * sizeof(table_4_3_exp[0]));
        if(!table_4_3_exp)
	    return -1;
	table_4_3_value= av_mallocz_static(TABLE_4_3_SIZE * sizeof(table_4_3_value[0]));
        if(!table_4_3_value)
            return -1;
        
        int_pow_init();
        for(i=1;i<TABLE_4_3_SIZE;i++) {
            double f, fm;
            int e, m;
            f = pow((double)(i/4), 4.0 / 3.0) * pow(2, (i&3)*0.25);
            fm = frexp(f, &e);
            m = FIXHR(fm*0.5);
            e+= FRAC_BITS - 31;

            /
            table_4_3_value[i] = m;
//            av_log(NULL, AV_LOG_DEBUG, "%d %d %f\n", i, m, pow((double)i, 4.0 / 3.0));
            table_4_3_exp[i] = -e;
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
            csa_table[i][0] = FIXHR(cs/4);
            csa_table[i][1] = FIXHR(ca/4);
            csa_table[i][2] = FIXHR(ca/4) + FIXHR(cs/4);
            csa_table[i][3] = FIXHR(ca/4) - FIXHR(cs/4); 
            csa_table_float[i][0] = cs;
            csa_table_float[i][1] = ca;
            csa_table_float[i][2] = ca + cs;
            csa_table_float[i][3] = ca - cs; 
//            printf("%d %d %d %d\n", FIX(cs), FIX(cs-1), FIX(ca), FIX(cs)-FIX(ca));
//            av_log(NULL, AV_LOG_DEBUG,"%f %f %f %f\n", cs, ca, ca+cs, ca-cs);
        }

        /
        for(i=0;i<36;i++) {
            for(j=0; j<4; j++){
                double d;
                
                if(j==2 && i%3 != 1)
                    continue;
                
                d= sin(M_PI * (i + 0.5) / 36.0);
                if(j==1){
                    if     (i>=30) d= 0;
                    else if(i>=24) d= sin(M_PI * (i - 18 + 0.5) / 12.0);
                    else if(i>=18) d= 1;
                }else if(j==3){
                    if     (i<  6) d= 0;
                    else if(i< 12) d= sin(M_PI * (i -  6 + 0.5) / 12.0);
                    else if(i< 18) d= 1;
                }
                //merge last stage of imdct into the window coefficients
                d*= 0.5 / cos(M_PI*(2*i + 19)/72);

                if(j==2)
                    mdct_win[j][i/3] = FIXHR((d / (1<<5)));
                else
                    mdct_win[j][i  ] = FIXHR((d / (1<<5)));
//                av_log(NULL, AV_LOG_DEBUG, "%2d %d %f\n", i,j,d / (1<<5));
            }
        }

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
    if (avctx->codec_id == CODEC_ID_MP3ADU)
        s->adu_mode = 1;
    return 0;
}
