int ff_h264_decode_mb_cavlc(H264Context *h){
    MpegEncContext * const s = &h->s;
    int mb_xy;
    int partition_count;
    unsigned int mb_type, cbp;
    int dct8x8_allowed= h->pps.transform_8x8_mode;

    mb_xy = h->mb_xy = s->mb_x + s->mb_y*s->mb_stride;

    tprintf(s->avctx, "pic:%d mb:%d/%d\n", h->frame_num, s->mb_x, s->mb_y);
    cbp = 0; /
    if(h->slice_type_nos != FF_I_TYPE){
        if(s->mb_skip_run==-1)
            s->mb_skip_run= get_ue_golomb(&s->gb);

        if (s->mb_skip_run--) {
            if(FRAME_MBAFF && (s->mb_y&1) == 0){
                if(s->mb_skip_run==0)
                    h->mb_mbaff = h->mb_field_decoding_flag = get_bits1(&s->gb);
                else
                    predict_field_decoding_flag(h);
            }
            decode_mb_skip(h);
            return 0;
        }
    }
    if(FRAME_MBAFF){
        if( (s->mb_y&1) == 0 )
            h->mb_mbaff = h->mb_field_decoding_flag = get_bits1(&s->gb);
    }

    h->prev_mb_skipped= 0;

    mb_type= get_ue_golomb(&s->gb);
    if(h->slice_type_nos == FF_B_TYPE){
        if(mb_type < 23){
            partition_count= b_mb_type_info[mb_type].partition_count;
            mb_type=         b_mb_type_info[mb_type].type;
        }else{
            mb_type -= 23;
            goto decode_intra_mb;
        }
    }else if(h->slice_type_nos == FF_P_TYPE){
        if(mb_type < 5){
            partition_count= p_mb_type_info[mb_type].partition_count;
            mb_type=         p_mb_type_info[mb_type].type;
        }else{
            mb_type -= 5;
            goto decode_intra_mb;
        }
    }else{
       assert(h->slice_type_nos == FF_I_TYPE);
        if(h->slice_type == FF_SI_TYPE && mb_type)
            mb_type--;
decode_intra_mb:
        if(mb_type > 25){
            av_log(h->s.avctx, AV_LOG_ERROR, "mb_type %d in %c slice too large at %d %d\n", mb_type, av_get_pict_type_char(h->slice_type), s->mb_x, s->mb_y);
            return -1;
        }
        partition_count=0;
        cbp= i_mb_type_info[mb_type].cbp;
        h->intra16x16_pred_mode= i_mb_type_info[mb_type].pred_mode;
        mb_type= i_mb_type_info[mb_type].type;
    }

    if(MB_FIELD)
        mb_type |= MB_TYPE_INTERLACED;

    h->slice_table[ mb_xy ]= h->slice_num;

    if(IS_INTRA_PCM(mb_type)){
        unsigned int x;

        // We assume these blocks are very rare so we do not optimize it.
        align_get_bits(&s->gb);

        // The pixels are stored in the same order as levels in h->mb array.
        for(x=0; x < (CHROMA ? 384 : 256); x++){
            ((uint8_t*)h->mb)[x]= get_bits(&s->gb, 8);
        }

        // In deblocking, the quantizer is 0
        s->current_picture.qscale_table[mb_xy]= 0;
        // All coeffs are present
        memset(h->non_zero_count[mb_xy], 16, 32);

        s->current_picture.mb_type[mb_xy]= mb_type;
        return 0;
    }

    if(MB_MBAFF){
        h->ref_count[0] <<= 1;
        h->ref_count[1] <<= 1;
    }

    fill_decode_caches(h, mb_type);

    //mb_pred
    if(IS_INTRA(mb_type)){
        int pred_mode;
//            init_top_left_availability(h);
        if(IS_INTRA4x4(mb_type)){
            int i;
            int di = 1;
            if(dct8x8_allowed && get_bits1(&s->gb)){
                mb_type |= MB_TYPE_8x8DCT;
                di = 4;
            }

//                fill_intra4x4_pred_table(h);
            for(i=0; i<16; i+=di){
                int mode= pred_intra_mode(h, i);

                if(!get_bits1(&s->gb)){
                    const int rem_mode= get_bits(&s->gb, 3);
                    mode = rem_mode + (rem_mode >= mode);
                }

                if(di==4)
                    fill_rectangle( &h->intra4x4_pred_mode_cache[ scan8[i] ], 2, 2, 8, mode, 1 );
                else
                    h->intra4x4_pred_mode_cache[ scan8[i] ] = mode;
            }
            ff_h264_write_back_intra_pred_mode(h);
            if( ff_h264_check_intra4x4_pred_mode(h) < 0)
                return -1;
        }else{
            h->intra16x16_pred_mode= ff_h264_check_intra_pred_mode(h, h->intra16x16_pred_mode);
            if(h->intra16x16_pred_mode < 0)
                return -1;
        }
        if(CHROMA){
            pred_mode= ff_h264_check_intra_pred_mode(h, get_ue_golomb_31(&s->gb));
            if(pred_mode < 0)
                return -1;
            h->chroma_pred_mode= pred_mode;
        }
    }else if(partition_count==4){
        int i, j, sub_partition_count[4], list, ref[2][4];

        if(h->slice_type_nos == FF_B_TYPE){
            for(i=0; i<4; i++){
                h->sub_mb_type[i]= get_ue_golomb_31(&s->gb);
                if(h->sub_mb_type[i] >=13){
                    av_log(h->s.avctx, AV_LOG_ERROR, "B sub_mb_type %u out of range at %d %d\n", h->sub_mb_type[i], s->mb_x, s->mb_y);
                    return -1;
                }
                sub_partition_count[i]= b_sub_mb_type_info[ h->sub_mb_type[i] ].partition_count;
                h->sub_mb_type[i]=      b_sub_mb_type_info[ h->sub_mb_type[i] ].type;
            }
            if( IS_DIRECT(h->sub_mb_type[0]|h->sub_mb_type[1]|h->sub_mb_type[2]|h->sub_mb_type[3])) {
                ff_h264_pred_direct_motion(h, &mb_type);
                h->ref_cache[0][scan8[4]] =
                h->ref_cache[1][scan8[4]] =
                h->ref_cache[0][scan8[12]] =
                h->ref_cache[1][scan8[12]] = PART_NOT_AVAILABLE;
            }
        }else{
            assert(h->slice_type_nos == FF_P_TYPE); //FIXME SP correct ?
            for(i=0; i<4; i++){
                h->sub_mb_type[i]= get_ue_golomb_31(&s->gb);
                if(h->sub_mb_type[i] >=4){
                    av_log(h->s.avctx, AV_LOG_ERROR, "P sub_mb_type %u out of range at %d %d\n", h->sub_mb_type[i], s->mb_x, s->mb_y);
                    return -1;
                }
                sub_partition_count[i]= p_sub_mb_type_info[ h->sub_mb_type[i] ].partition_count;
                h->sub_mb_type[i]=      p_sub_mb_type_info[ h->sub_mb_type[i] ].type;
            }
        }

        for(list=0; list<h->list_count; list++){
            int ref_count= IS_REF0(mb_type) ? 1 : h->ref_count[list];
            for(i=0; i<4; i++){
                if(IS_DIRECT(h->sub_mb_type[i])) continue;
                if(IS_DIR(h->sub_mb_type[i], 0, list)){
                    unsigned int tmp;
                    if(ref_count == 1){
                        tmp= 0;
                    }else if(ref_count == 2){
                        tmp= get_bits1(&s->gb)^1;
                    }else{
                        tmp= get_ue_golomb_31(&s->gb);
                        if(tmp>=ref_count){
                            av_log(h->s.avctx, AV_LOG_ERROR, "ref %u overflow\n", tmp);
                            return -1;
                        }
                    }
                    ref[list][i]= tmp;
                }else{
                 //FIXME
                    ref[list][i] = -1;
                }
            }
        }

        if(dct8x8_allowed)
            dct8x8_allowed = get_dct8x8_allowed(h);

        for(list=0; list<h->list_count; list++){
            for(i=0; i<4; i++){
                if(IS_DIRECT(h->sub_mb_type[i])) {
                    h->ref_cache[list][ scan8[4*i] ] = h->ref_cache[list][ scan8[4*i]+1 ];
                    continue;
                }
                h->ref_cache[list][ scan8[4*i]   ]=h->ref_cache[list][ scan8[4*i]+1 ]=
                h->ref_cache[list][ scan8[4*i]+8 ]=h->ref_cache[list][ scan8[4*i]+9 ]= ref[list][i];

                if(IS_DIR(h->sub_mb_type[i], 0, list)){
                    const int sub_mb_type= h->sub_mb_type[i];
                    const int block_width= (sub_mb_type & (MB_TYPE_16x16|MB_TYPE_16x8)) ? 2 : 1;
                    for(j=0; j<sub_partition_count[i]; j++){
                        int mx, my;
                        const int index= 4*i + block_width*j;
                        int16_t (* mv_cache)[2]= &h->mv_cache[list][ scan8[index] ];
                        pred_motion(h, index, block_width, list, h->ref_cache[list][ scan8[index] ], &mx, &my);
                        mx += get_se_golomb(&s->gb);
                        my += get_se_golomb(&s->gb);
                        tprintf(s->avctx, "final mv:%d %d\n", mx, my);

                        if(IS_SUB_8X8(sub_mb_type)){
                            mv_cache[ 1 ][0]=
                            mv_cache[ 8 ][0]= mv_cache[ 9 ][0]= mx;
                            mv_cache[ 1 ][1]=
                            mv_cache[ 8 ][1]= mv_cache[ 9 ][1]= my;
                        }else if(IS_SUB_8X4(sub_mb_type)){
                            mv_cache[ 1 ][0]= mx;
                            mv_cache[ 1 ][1]= my;
                        }else if(IS_SUB_4X8(sub_mb_type)){
                            mv_cache[ 8 ][0]= mx;
                            mv_cache[ 8 ][1]= my;
                        }
                        mv_cache[ 0 ][0]= mx;
                        mv_cache[ 0 ][1]= my;
                    }
                }else{
                    uint32_t *p= (uint32_t *)&h->mv_cache[list][ scan8[4*i] ][0];
                    p[0] = p[1]=
                    p[8] = p[9]= 0;
                }
            }
        }
    }else if(IS_DIRECT(mb_type)){
        ff_h264_pred_direct_motion(h, &mb_type);
        dct8x8_allowed &= h->sps.direct_8x8_inference_flag;
    }else{
        int list, mx, my, i;
         //FIXME we should set ref_idx_l? to 0 if we use that later ...
        if(IS_16X16(mb_type)){
            for(list=0; list<h->list_count; list++){
                    unsigned int val;
                    if(IS_DIR(mb_type, 0, list)){
                        if(h->ref_count[list]==1){
                            val= 0;
                        }else if(h->ref_count[list]==2){
                            val= get_bits1(&s->gb)^1;
                        }else{
                            val= get_ue_golomb_31(&s->gb);
                            if(val >= h->ref_count[list]){
                                av_log(h->s.avctx, AV_LOG_ERROR, "ref %u overflow\n", val);
                                return -1;
                            }
                        }
                    }else
                        val= LIST_NOT_USED&0xFF;
                    fill_rectangle(&h->ref_cache[list][ scan8[0] ], 4, 4, 8, val, 1);
            }
            for(list=0; list<h->list_count; list++){
                unsigned int val;
                if(IS_DIR(mb_type, 0, list)){
                    pred_motion(h, 0, 4, list, h->ref_cache[list][ scan8[0] ], &mx, &my);
                    mx += get_se_golomb(&s->gb);
                    my += get_se_golomb(&s->gb);
                    tprintf(s->avctx, "final mv:%d %d\n", mx, my);

                    val= pack16to32(mx,my);
                }else
                    val=0;
                fill_rectangle(h->mv_cache[list][ scan8[0] ], 4, 4, 8, val, 4);
            }
        }
        else if(IS_16X8(mb_type)){
            for(list=0; list<h->list_count; list++){
                    for(i=0; i<2; i++){
                        unsigned int val;
                        if(IS_DIR(mb_type, i, list)){
                            if(h->ref_count[list] == 1){
                                val= 0;
                            }else if(h->ref_count[list] == 2){
                                val= get_bits1(&s->gb)^1;
                            }else{
                                val= get_ue_golomb_31(&s->gb);
                                if(val >= h->ref_count[list]){
                                    av_log(h->s.avctx, AV_LOG_ERROR, "ref %u overflow\n", val);
                                    return -1;
                                }
                            }
                        }else
                            val= LIST_NOT_USED&0xFF;
                        fill_rectangle(&h->ref_cache[list][ scan8[0] + 16*i ], 4, 2, 8, val, 1);
                    }
            }
            for(list=0; list<h->list_count; list++){
                for(i=0; i<2; i++){
                    unsigned int val;
                    if(IS_DIR(mb_type, i, list)){
                        pred_16x8_motion(h, 8*i, list, h->ref_cache[list][scan8[0] + 16*i], &mx, &my);
                        mx += get_se_golomb(&s->gb);
                        my += get_se_golomb(&s->gb);
                        tprintf(s->avctx, "final mv:%d %d\n", mx, my);

                        val= pack16to32(mx,my);
                    }else
                        val=0;
                    fill_rectangle(h->mv_cache[list][ scan8[0] + 16*i ], 4, 2, 8, val, 4);
                }
            }
        }else{
            assert(IS_8X16(mb_type));
            for(list=0; list<h->list_count; list++){
                    for(i=0; i<2; i++){
                        unsigned int val;
                        if(IS_DIR(mb_type, i, list)){ //FIXME optimize
                            if(h->ref_count[list]==1){
                                val= 0;
                            }else if(h->ref_count[list]==2){
                                val= get_bits1(&s->gb)^1;
                            }else{
                                val= get_ue_golomb_31(&s->gb);
                                if(val >= h->ref_count[list]){
                                    av_log(h->s.avctx, AV_LOG_ERROR, "ref %u overflow\n", val);
                                    return -1;
                                }
                            }
                        }else
                            val= LIST_NOT_USED&0xFF;
                        fill_rectangle(&h->ref_cache[list][ scan8[0] + 2*i ], 2, 4, 8, val, 1);
                    }
            }
            for(list=0; list<h->list_count; list++){
                for(i=0; i<2; i++){
                    unsigned int val;
                    if(IS_DIR(mb_type, i, list)){
                        pred_8x16_motion(h, i*4, list, h->ref_cache[list][ scan8[0] + 2*i ], &mx, &my);
                        mx += get_se_golomb(&s->gb);
                        my += get_se_golomb(&s->gb);
                        tprintf(s->avctx, "final mv:%d %d\n", mx, my);

                        val= pack16to32(mx,my);
                    }else
                        val=0;
                    fill_rectangle(h->mv_cache[list][ scan8[0] + 2*i ], 2, 4, 8, val, 4);
                }
            }
        }
    }

    if(IS_INTER(mb_type))
        write_back_motion(h, mb_type);

    if(!IS_INTRA16x16(mb_type)){
        cbp= get_ue_golomb(&s->gb);
        if(cbp > 47){
            av_log(h->s.avctx, AV_LOG_ERROR, "cbp too large (%u) at %d %d\n", cbp, s->mb_x, s->mb_y);
            return -1;
        }

        if(CHROMA){
            if(IS_INTRA4x4(mb_type)) cbp= golomb_to_intra4x4_cbp[cbp];
            else                     cbp= golomb_to_inter_cbp   [cbp];
        }else{
            if(IS_INTRA4x4(mb_type)) cbp= golomb_to_intra4x4_cbp_gray[cbp];
            else                     cbp= golomb_to_inter_cbp_gray[cbp];
        }
    }

    if(dct8x8_allowed && (cbp&15) && !IS_INTRA(mb_type)){
        if(get_bits1(&s->gb)){
            mb_type |= MB_TYPE_8x8DCT;
        }
    }
    h->cbp=
    h->cbp_table[mb_xy]= cbp;
    s->current_picture.mb_type[mb_xy]= mb_type;

    if(cbp || IS_INTRA16x16(mb_type)){
        int i8x8, i4x4, chroma_idx;
        int dquant;
        GetBitContext *gb= IS_INTRA(mb_type) ? h->intra_gb_ptr : h->inter_gb_ptr;
        const uint8_t *scan, *scan8x8, *dc_scan;

//        fill_non_zero_count_cache(h);

        if(IS_INTERLACED(mb_type)){
            scan8x8= s->qscale ? h->field_scan8x8_cavlc : h->field_scan8x8_cavlc_q0;
            scan= s->qscale ? h->field_scan : h->field_scan_q0;
            dc_scan= luma_dc_field_scan;
        }else{
            scan8x8= s->qscale ? h->zigzag_scan8x8_cavlc : h->zigzag_scan8x8_cavlc_q0;
            scan= s->qscale ? h->zigzag_scan : h->zigzag_scan_q0;
            dc_scan= luma_dc_zigzag_scan;
        }

        dquant= get_se_golomb(&s->gb);

        if( dquant > 25 || dquant < -26 ){
            av_log(h->s.avctx, AV_LOG_ERROR, "dquant out of range (%d) at %d %d\n", dquant, s->mb_x, s->mb_y);
            return -1;
        }

        s->qscale += dquant;
        if(((unsigned)s->qscale) > 51){
            if(s->qscale<0) s->qscale+= 52;
            else            s->qscale-= 52;
        }

        h->chroma_qp[0]= get_chroma_qp(h, 0, s->qscale);
        h->chroma_qp[1]= get_chroma_qp(h, 1, s->qscale);
        if(IS_INTRA16x16(mb_type)){
            if( decode_residual(h, h->intra_gb_ptr, h->mb, LUMA_DC_BLOCK_INDEX, dc_scan, h->dequant4_coeff[0][s->qscale], 16) < 0){
                return -1; //FIXME continue if partitioned and other return -1 too
            }

            assert((cbp&15) == 0 || (cbp&15) == 15);

            if(cbp&15){
                for(i8x8=0; i8x8<4; i8x8++){
                    for(i4x4=0; i4x4<4; i4x4++){
                        const int index= i4x4 + 4*i8x8;
                        if( decode_residual(h, h->intra_gb_ptr, h->mb + 16*index, index, scan + 1, h->dequant4_coeff[0][s->qscale], 15) < 0 ){
                            return -1;
                        }
                    }
                }
            }else{
                fill_rectangle(&h->non_zero_count_cache[scan8[0]], 4, 4, 8, 0, 1);
            }
        }else{
            for(i8x8=0; i8x8<4; i8x8++){
                if(cbp & (1<<i8x8)){
                    if(IS_8x8DCT(mb_type)){
                        DCTELEM *buf = &h->mb[64*i8x8];
                        uint8_t *nnz;
                        for(i4x4=0; i4x4<4; i4x4++){
                            if( decode_residual(h, gb, buf, i4x4+4*i8x8, scan8x8+16*i4x4,
                                                h->dequant8_coeff[IS_INTRA( mb_type ) ? 0:1][s->qscale], 16) <0 )
                                return -1;
                        }
                        nnz= &h->non_zero_count_cache[ scan8[4*i8x8] ];
                        nnz[0] += nnz[1] + nnz[8] + nnz[9];
                    }else{
                        for(i4x4=0; i4x4<4; i4x4++){
                            const int index= i4x4 + 4*i8x8;

                            if( decode_residual(h, gb, h->mb + 16*index, index, scan, h->dequant4_coeff[IS_INTRA( mb_type ) ? 0:3][s->qscale], 16) <0 ){
                                return -1;
                            }
                        }
                    }
                }else{
                    uint8_t * const nnz= &h->non_zero_count_cache[ scan8[4*i8x8] ];
                    nnz[0] = nnz[1] = nnz[8] = nnz[9] = 0;
                }
            }
        }

        if(cbp&0x30){
            for(chroma_idx=0; chroma_idx<2; chroma_idx++)
                if( decode_residual(h, gb, h->mb + 256 + 16*4*chroma_idx, CHROMA_DC_BLOCK_INDEX, chroma_dc_scan, NULL, 4) < 0){
                    return -1;
                }
        }

        if(cbp&0x20){
            for(chroma_idx=0; chroma_idx<2; chroma_idx++){
                const uint32_t *qmul = h->dequant4_coeff[chroma_idx+1+(IS_INTRA( mb_type ) ? 0:3)][h->chroma_qp[chroma_idx]];
                for(i4x4=0; i4x4<4; i4x4++){
                    const int index= 16 + 4*chroma_idx + i4x4;
                    if( decode_residual(h, gb, h->mb + 16*index, index, scan + 1, qmul, 15) < 0){
                        return -1;
                    }
                }
            }
        }else{
            uint8_t * const nnz= &h->non_zero_count_cache[0];
            nnz[ scan8[16]+0 ] = nnz[ scan8[16]+1 ] =nnz[ scan8[16]+8 ] =nnz[ scan8[16]+9 ] =
            nnz[ scan8[20]+0 ] = nnz[ scan8[20]+1 ] =nnz[ scan8[20]+8 ] =nnz[ scan8[20]+9 ] = 0;
        }
    }else{
        uint8_t * const nnz= &h->non_zero_count_cache[0];
        fill_rectangle(&nnz[scan8[0]], 4, 4, 8, 0, 1);
        nnz[ scan8[16]+0 ] = nnz[ scan8[16]+1 ] =nnz[ scan8[16]+8 ] =nnz[ scan8[16]+9 ] =
        nnz[ scan8[20]+0 ] = nnz[ scan8[20]+1 ] =nnz[ scan8[20]+8 ] =nnz[ scan8[20]+9 ] = 0;
    }
    s->current_picture.qscale_table[mb_xy]= s->qscale;
    write_back_non_zero_count(h);

    if(MB_MBAFF){
        h->ref_count[0] >>= 1;
        h->ref_count[1] >>= 1;
    }

    return 0;
}
