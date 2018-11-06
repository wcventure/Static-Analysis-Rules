static inline int mpeg4_decode_block(MpegEncContext * s, DCTELEM * block,
                              int n, int coded, int intra)
{
    int level, i, last, run;
    int dc_pred_dir;
    RLTable * rl;
    RL_VLC_ELEM * rl_vlc;
    const UINT8 * scan_table;
    int qmul, qadd;

    if(intra) {
	/
        if(s->data_partitioning && s->pict_type!=B_TYPE){
            level = s->dc_val[0][ s->block_index[n] ];
            if(n<4) level= (level + (s->y_dc_scale>>1))/s->y_dc_scale; //FIXME optimizs
            else    level= (level + (s->c_dc_scale>>1))/s->c_dc_scale;
            dc_pred_dir= (s->pred_dir_table[s->mb_x + s->mb_y*s->mb_width]<<n)&32;
        }else{
            level = mpeg4_decode_dc(s, n, &dc_pred_dir);
            if (level < 0)
                return DECODING_ACDC_LOST;
        }
        block[0] = level;
        i = 0;
        if (!coded) 
            goto not_coded;
        rl = &rl_intra;
        rl_vlc = rl_intra.rl_vlc[0];
        if (s->ac_pred) {
            if (dc_pred_dir == 0) 
                scan_table = ff_alternate_vertical_scan; /
            else
                scan_table = ff_alternate_horizontal_scan; /
        } else {
            scan_table = zigzag_direct;
        }
        qmul=1;
        qadd=0;
    } else {
        i = -1;
        if (!coded) {
            s->block_last_index[n] = i;
            return 0;
        }
        rl = &rl_inter;
        scan_table = zigzag_direct;
        if(s->mpeg_quant){
            qmul=1;
            qadd=0;
            rl_vlc = rl_inter.rl_vlc[0];        
        }else{
            qmul = s->qscale << 1;
            qadd = (s->qscale - 1) | 1;
            rl_vlc = rl_inter.rl_vlc[s->qscale];
        }
    }
  {
    OPEN_READER(re, &s->gb);
    for(;;) {
        UPDATE_CACHE(re, &s->gb);
        GET_RL_VLC(level, run, re, &s->gb, rl_vlc, TEX_VLC_BITS, 2);
        if (level==0) {
            int cache;
            cache= GET_CACHE(re, &s->gb);
            /
            if (cache&0x80000000) {
                if (cache&0x40000000) {
                    /
                    SKIP_CACHE(re, &s->gb, 2);
                    last=  SHOW_UBITS(re, &s->gb, 1); SKIP_CACHE(re, &s->gb, 1);
                    run=   SHOW_UBITS(re, &s->gb, 6); LAST_SKIP_CACHE(re, &s->gb, 6);
                    SKIP_COUNTER(re, &s->gb, 2+1+6);
                    UPDATE_CACHE(re, &s->gb);

                    if(SHOW_UBITS(re, &s->gb, 1)==0){
                        fprintf(stderr, "1. marker bit missing in 3. esc\n");
                        return DECODING_AC_LOST;
                    }; SKIP_CACHE(re, &s->gb, 1);
                    
                    level= SHOW_SBITS(re, &s->gb, 12); SKIP_CACHE(re, &s->gb, 12);
 
                    if(SHOW_UBITS(re, &s->gb, 1)==0){
                        fprintf(stderr, "2. marker bit missing in 3. esc\n");
                        return DECODING_AC_LOST;
                    }; LAST_SKIP_CACHE(re, &s->gb, 1);
                    
                    SKIP_COUNTER(re, &s->gb, 1+12+1);
                    
                    if(level>512 || level<-512){ //FIXME check that QP=1 is ok with this too
                        fprintf(stderr, "|level| overflow in 3. esc\n");
                        return DECODING_AC_LOST;
                    }
#if 1 
                    {
                        const int abs_level= ABS(level);
                        if(abs_level<=MAX_LEVEL && run<=MAX_RUN && s->error_resilience>=0){
                            const int run1= run - rl->max_run[last][abs_level] - 1;
                            if(abs_level <= rl->max_level[last][run]){
                                fprintf(stderr, "illegal 3. esc, vlc encoding possible\n");
                                return DECODING_AC_LOST;
                            }
                            if(abs_level <= rl->max_level[last][run]*2){
                                fprintf(stderr, "illegal 3. esc, esc 1 encoding possible\n");
                                return DECODING_AC_LOST;
                            }
                            if(run1 >= 0 && abs_level <= rl->max_level[last][run1]){
                                fprintf(stderr, "illegal 3. esc, esc 2 encoding possible\n");
                                return DECODING_AC_LOST;
                            }
                        }
                    }
#endif
		    if (level>0) level= level * qmul + qadd;
                    else         level= level * qmul - qadd;

                    i+= run + 1;
                    if(last) i+=192;
                } else {
                    /
#if MIN_CACHE_BITS < 20
                    LAST_SKIP_BITS(re, &s->gb, 2);
                    UPDATE_CACHE(re, &s->gb);
#else
                    SKIP_BITS(re, &s->gb, 2);
#endif
                    GET_RL_VLC(level, run, re, &s->gb, rl_vlc, TEX_VLC_BITS, 2);
                    i+= run + rl->max_run[run>>7][level/qmul] +1; //FIXME opt indexing
                    level = (level ^ SHOW_SBITS(re, &s->gb, 1)) - SHOW_SBITS(re, &s->gb, 1);
                    LAST_SKIP_BITS(re, &s->gb, 1);
                }
            } else {
                /
#if MIN_CACHE_BITS < 19
                LAST_SKIP_BITS(re, &s->gb, 1);
                UPDATE_CACHE(re, &s->gb);
#else
                SKIP_BITS(re, &s->gb, 1);
#endif
                GET_RL_VLC(level, run, re, &s->gb, rl_vlc, TEX_VLC_BITS, 2);
                i+= run;
                level = level + rl->max_level[run>>7][(run-1)&63] * qmul;//FIXME opt indexing
                level = (level ^ SHOW_SBITS(re, &s->gb, 1)) - SHOW_SBITS(re, &s->gb, 1);
                LAST_SKIP_BITS(re, &s->gb, 1);
            }
        } else {
            i+= run;
            level = (level ^ SHOW_SBITS(re, &s->gb, 1)) - SHOW_SBITS(re, &s->gb, 1);
            LAST_SKIP_BITS(re, &s->gb, 1);
        }
        if (i > 62){
            i-= 192;
            if(i&(~63)){
                fprintf(stderr, "ac-tex damaged at %d %d\n", s->mb_x, s->mb_y);
                return DECODING_AC_LOST;
            }

            block[scan_table[i]] = level;
            break;
        }

        block[scan_table[i]] = level;
    }
    CLOSE_READER(re, &s->gb);
  }
 not_coded:
    if (s->mb_intra) {
        mpeg4_pred_ac(s, block, n, dc_pred_dir);
        if (s->ac_pred) {
            i = 63; /
        }
    }
    s->block_last_index[n] = i;
    return 0;
}
