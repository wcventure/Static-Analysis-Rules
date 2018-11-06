static inline void pred_direct_motion(H264Context * const h, int *mb_type){
    MpegEncContext * const s = &h->s;
    const int mb_xy =   s->mb_x +   s->mb_y*s->mb_stride;
    const int b8_xy = 2*s->mb_x + 2*s->mb_y*h->b8_stride;
    const int b4_xy = 4*s->mb_x + 4*s->mb_y*h->b_stride;
    const int mb_type_col = h->ref_list[1][0].mb_type[mb_xy];
    const int16_t (*l1mv0)[2] = (const int16_t (*)[2]) &h->ref_list[1][0].motion_val[0][b4_xy];
    const int16_t (*l1mv1)[2] = (const int16_t (*)[2]) &h->ref_list[1][0].motion_val[1][b4_xy];
    const int8_t *l1ref0 = &h->ref_list[1][0].ref_index[0][b8_xy];
    const int8_t *l1ref1 = &h->ref_list[1][0].ref_index[1][b8_xy];
    const int is_b8x8 = IS_8X8(*mb_type);
    int sub_mb_type;
    int i8, i4;

    if(IS_8X8(mb_type_col) && !h->sps.direct_8x8_inference_flag){
        /
        sub_mb_type = MB_TYPE_8x8|MB_TYPE_P0L0|MB_TYPE_P0L1|MB_TYPE_DIRECT2; /
        *mb_type =    MB_TYPE_8x8|MB_TYPE_L0L1;
    }else if(!is_b8x8 && (IS_16X16(mb_type_col) || IS_INTRA(mb_type_col))){
        sub_mb_type = MB_TYPE_16x16|MB_TYPE_P0L0|MB_TYPE_P0L1|MB_TYPE_DIRECT2; /
        *mb_type =    MB_TYPE_16x16|MB_TYPE_P0L0|MB_TYPE_P0L1|MB_TYPE_DIRECT2; /
    }else{
        sub_mb_type = MB_TYPE_16x16|MB_TYPE_P0L0|MB_TYPE_P0L1|MB_TYPE_DIRECT2; /
        *mb_type =    MB_TYPE_8x8|MB_TYPE_L0L1;
    }
    if(!is_b8x8)
        *mb_type |= MB_TYPE_DIRECT2;

    tprintf("mb_type = %08x, sub_mb_type = %08x, is_b8x8 = %d, mb_type_col = %08x\n", *mb_type, sub_mb_type, is_b8x8, mb_type_col);

    if(h->direct_spatial_mv_pred){
        int ref[2];
        int mv[2][2];
        int list;

        /
        for(list=0; list<2; list++){
            int refa = h->ref_cache[list][scan8[0] - 1];
            int refb = h->ref_cache[list][scan8[0] - 8];
            int refc = h->ref_cache[list][scan8[0] - 8 + 4];
            if(refc == -2)
                refc = h->ref_cache[list][scan8[0] - 8 - 1];
            ref[list] = refa;
            if(ref[list] < 0 || (refb < ref[list] && refb >= 0))
                ref[list] = refb;
            if(ref[list] < 0 || (refc < ref[list] && refc >= 0))
                ref[list] = refc;
            if(ref[list] < 0)
                ref[list] = -1;
        }

        if(ref[0] < 0 && ref[1] < 0){
            ref[0] = ref[1] = 0;
            mv[0][0] = mv[0][1] =
            mv[1][0] = mv[1][1] = 0;
        }else{
            for(list=0; list<2; list++){
                if(ref[list] >= 0)
                    pred_motion(h, 0, 4, list, ref[list], &mv[list][0], &mv[list][1]);
                else
                    mv[list][0] = mv[list][1] = 0;
            }
        }

        if(ref[1] < 0){
            *mb_type &= ~MB_TYPE_P0L1;
            sub_mb_type &= ~MB_TYPE_P0L1;
        }else if(ref[0] < 0){
            *mb_type &= ~MB_TYPE_P0L0;
            sub_mb_type &= ~MB_TYPE_P0L0;
        }

        if(IS_16X16(*mb_type)){
            fill_rectangle(&h->ref_cache[0][scan8[0]], 4, 4, 8, ref[0], 1);
            fill_rectangle(&h->ref_cache[1][scan8[0]], 4, 4, 8, ref[1], 1);
            if(!IS_INTRA(mb_type_col)
               && (   (l1ref0[0] == 0 && ABS(l1mv0[0][0]) <= 1 && ABS(l1mv0[0][1]) <= 1)
                   || (l1ref0[0]  < 0 && l1ref1[0] == 0 && ABS(l1mv1[0][0]) <= 1 && ABS(l1mv1[0][1]) <= 1
                       && (h->x264_build>33 || !h->x264_build)))){
                if(ref[0] > 0)
                    fill_rectangle(&h->mv_cache[0][scan8[0]], 4, 4, 8, pack16to32(mv[0][0],mv[0][1]), 4);
                else
                    fill_rectangle(&h->mv_cache[0][scan8[0]], 4, 4, 8, 0, 4);
                if(ref[1] > 0)
                    fill_rectangle(&h->mv_cache[1][scan8[0]], 4, 4, 8, pack16to32(mv[1][0],mv[1][1]), 4);
                else
                    fill_rectangle(&h->mv_cache[1][scan8[0]], 4, 4, 8, 0, 4);
            }else{
                fill_rectangle(&h->mv_cache[0][scan8[0]], 4, 4, 8, pack16to32(mv[0][0],mv[0][1]), 4);
                fill_rectangle(&h->mv_cache[1][scan8[0]], 4, 4, 8, pack16to32(mv[1][0],mv[1][1]), 4);
            }
        }else{
            for(i8=0; i8<4; i8++){
                const int x8 = i8&1;
                const int y8 = i8>>1;

                if(is_b8x8 && !IS_DIRECT(h->sub_mb_type[i8]))
                    continue;
                h->sub_mb_type[i8] = sub_mb_type;

                fill_rectangle(&h->mv_cache[0][scan8[i8*4]], 2, 2, 8, pack16to32(mv[0][0],mv[0][1]), 4);
                fill_rectangle(&h->mv_cache[1][scan8[i8*4]], 2, 2, 8, pack16to32(mv[1][0],mv[1][1]), 4);
                fill_rectangle(&h->ref_cache[0][scan8[i8*4]], 2, 2, 8, ref[0], 1);
                fill_rectangle(&h->ref_cache[1][scan8[i8*4]], 2, 2, 8, ref[1], 1);

                /
                if(!IS_INTRA(mb_type_col) && (   l1ref0[x8 + y8*h->b8_stride] == 0
                                              || (l1ref0[x8 + y8*h->b8_stride] < 0 && l1ref1[x8 + y8*h->b8_stride] == 0
                                                  && (h->x264_build>33 || !h->x264_build)))){
                    const int16_t (*l1mv)[2]= l1ref0[x8 + y8*h->b8_stride] == 0 ? l1mv0 : l1mv1;
                    for(i4=0; i4<4; i4++){
                        const int16_t *mv_col = l1mv[x8*2 + (i4&1) + (y8*2 + (i4>>1))*h->b_stride];
                        if(ABS(mv_col[0]) <= 1 && ABS(mv_col[1]) <= 1){
                            if(ref[0] == 0)
                                *(uint32_t*)h->mv_cache[0][scan8[i8*4+i4]] = 0;
                            if(ref[1] == 0)
                                *(uint32_t*)h->mv_cache[1][scan8[i8*4+i4]] = 0;
                        }
                    }
                }
            }
        }
    }else{ /
        if(IS_16X16(*mb_type)){
            fill_rectangle(&h->ref_cache[1][scan8[0]], 4, 4, 8, 0, 1);
            if(IS_INTRA(mb_type_col)){
                fill_rectangle(&h->ref_cache[0][scan8[0]], 4, 4, 8, 0, 1);
                fill_rectangle(&h-> mv_cache[0][scan8[0]], 4, 4, 8, 0, 4);
                fill_rectangle(&h-> mv_cache[1][scan8[0]], 4, 4, 8, 0, 4);
            }else{
                const int ref0 = l1ref0[0] >= 0 ? h->map_col_to_list0[0][l1ref0[0]]
                                                : h->map_col_to_list0[1][l1ref1[0]];
                const int dist_scale_factor = h->dist_scale_factor[ref0];
                const int16_t *mv_col = l1ref0[0] >= 0 ? l1mv0[0] : l1mv1[0];
                int mv_l0[2];
                mv_l0[0] = (dist_scale_factor * mv_col[0] + 128) >> 8;
                mv_l0[1] = (dist_scale_factor * mv_col[1] + 128) >> 8;
                fill_rectangle(&h->ref_cache[0][scan8[0]], 4, 4, 8, ref0, 1);
                fill_rectangle(&h-> mv_cache[0][scan8[0]], 4, 4, 8, pack16to32(mv_l0[0],mv_l0[1]), 4);
                fill_rectangle(&h-> mv_cache[1][scan8[0]], 4, 4, 8, pack16to32(mv_l0[0]-mv_col[0],mv_l0[1]-mv_col[1]), 4);
            }
        }else{
            for(i8=0; i8<4; i8++){
                const int x8 = i8&1;
                const int y8 = i8>>1;
                int ref0, dist_scale_factor;
                const int16_t (*l1mv)[2]= l1mv0;

                if(is_b8x8 && !IS_DIRECT(h->sub_mb_type[i8]))
                    continue;
                h->sub_mb_type[i8] = sub_mb_type;
                if(IS_INTRA(mb_type_col)){
                    fill_rectangle(&h->ref_cache[0][scan8[i8*4]], 2, 2, 8, 0, 1);
                    fill_rectangle(&h->ref_cache[1][scan8[i8*4]], 2, 2, 8, 0, 1);
                    fill_rectangle(&h-> mv_cache[0][scan8[i8*4]], 2, 2, 8, 0, 4);
                    fill_rectangle(&h-> mv_cache[1][scan8[i8*4]], 2, 2, 8, 0, 4);
                    continue;
                }

                ref0 = l1ref0[x8 + y8*h->b8_stride];
                if(ref0 >= 0)
                    ref0 = h->map_col_to_list0[0][ref0];
                else{
                    ref0 = h->map_col_to_list0[1][l1ref1[x8 + y8*h->b8_stride]];
                    l1mv= l1mv1;
                }
                dist_scale_factor = h->dist_scale_factor[ref0];

                fill_rectangle(&h->ref_cache[0][scan8[i8*4]], 2, 2, 8, ref0, 1);
                fill_rectangle(&h->ref_cache[1][scan8[i8*4]], 2, 2, 8, 0, 1);
                for(i4=0; i4<4; i4++){
                    const int16_t *mv_col = l1mv[x8*2 + (i4&1) + (y8*2 + (i4>>1))*h->b_stride];
                    int16_t *mv_l0 = h->mv_cache[0][scan8[i8*4+i4]];
                    mv_l0[0] = (dist_scale_factor * mv_col[0] + 128) >> 8;
                    mv_l0[1] = (dist_scale_factor * mv_col[1] + 128) >> 8;
                    *(uint32_t*)h->mv_cache[1][scan8[i8*4+i4]] =
                        pack16to32(mv_l0[0]-mv_col[0],mv_l0[1]-mv_col[1]);
                }
            }
        }
    }
}
