int ff_h264_decode_mb_cavlc(const H264Context *h, H264SliceContext *sl)
{
    int mb_xy;
    int partition_count;
    unsigned int mb_type, cbp;
    int dct8x8_allowed= h->ps.pps->transform_8x8_mode;
    int decode_chroma = h->ps.sps->chroma_format_idc == 1 || h->ps.sps->chroma_format_idc == 2;
    const int pixel_shift = h->pixel_shift;

    mb_xy = sl->mb_xy = sl->mb_x + sl->mb_y*h->mb_stride;

    ff_tlog(h->avctx, "pic:%d mb:%d/%d\n", h->poc.frame_num, sl->mb_x, sl->mb_y);
    cbp = 0; /
    if (sl->slice_type_nos != AV_PICTURE_TYPE_I) {
        if (sl->mb_skip_run == -1)
            sl->mb_skip_run = get_ue_golomb_long(&sl->gb);

        if (sl->mb_skip_run--) {
            if (FRAME_MBAFF(h) && (sl->mb_y & 1) == 0) {
                if (sl->mb_skip_run == 0)
                    sl->mb_mbaff = sl->mb_field_decoding_flag = get_bits1(&sl->gb);
            }
            decode_mb_skip(h, sl);
            return 0;
        }
    }
    if (FRAME_MBAFF(h)) {
        if ((sl->mb_y & 1) == 0)
            sl->mb_mbaff = sl->mb_field_decoding_flag = get_bits1(&sl->gb);
    }

    sl->prev_mb_skipped = 0;

    mb_type= get_ue_golomb(&sl->gb);
    if (sl->slice_type_nos == AV_PICTURE_TYPE_B) {
        if(mb_type < 23){
            partition_count = ff_h264_b_mb_type_info[mb_type].partition_count;
            mb_type         = ff_h264_b_mb_type_info[mb_type].type;
        }else{
            mb_type -= 23;
            goto decode_intra_mb;
        }
    } else if (sl->slice_type_nos == AV_PICTURE_TYPE_P) {
        if(mb_type < 5){
            partition_count = ff_h264_p_mb_type_info[mb_type].partition_count;
            mb_type         = ff_h264_p_mb_type_info[mb_type].type;
        }else{
            mb_type -= 5;
            goto decode_intra_mb;
        }
    }else{
       av_assert2(sl->slice_type_nos == AV_PICTURE_TYPE_I);
        if (sl->slice_type == AV_PICTURE_TYPE_SI && mb_type)
            mb_type--;
decode_intra_mb:
        if(mb_type > 25){
            av_log(h->avctx, AV_LOG_ERROR, "mb_type %d in %c slice too large at %d %d\n", mb_type, av_get_picture_type_char(sl->slice_type), sl->mb_x, sl->mb_y);
            return -1;
        }
        partition_count=0;
        cbp                      = ff_h264_i_mb_type_info[mb_type].cbp;
        sl->intra16x16_pred_mode = ff_h264_i_mb_type_info[mb_type].pred_mode;
        mb_type                  = ff_h264_i_mb_type_info[mb_type].type;
    }

    if (MB_FIELD(sl))
        mb_type |= MB_TYPE_INTERLACED;

    h->slice_table[mb_xy] = sl->slice_num;

    if(IS_INTRA_PCM(mb_type)){
        const int mb_size = ff_h264_mb_sizes[h->ps.sps->chroma_format_idc] *
                            h->ps.sps->bit_depth_luma;

        // We assume these blocks are very rare so we do not optimize it.
        sl->intra_pcm_ptr = align_get_bits(&sl->gb);
        if (get_bits_left(&sl->gb) < mb_size) {
            av_log(h->avctx, AV_LOG_ERROR, "Not enough data for an intra PCM block.\n");
            return AVERROR_INVALIDDATA;
        }
        skip_bits_long(&sl->gb, mb_size);

        // In deblocking, the quantizer is 0
        h->cur_pic.qscale_table[mb_xy] = 0;
        // All coeffs are present
        memset(h->non_zero_count[mb_xy], 16, 48);

        h->cur_pic.mb_type[mb_xy] = mb_type;
        return 0;
    }

    fill_decode_neighbors(h, sl, mb_type);
    fill_decode_caches(h, sl, mb_type);

    //mb_pred
    if(IS_INTRA(mb_type)){
        int pred_mode;
//            init_top_left_availability(h);
        if(IS_INTRA4x4(mb_type)){
            int i;
            int di = 1;
            if(dct8x8_allowed && get_bits1(&sl->gb)){
                mb_type |= MB_TYPE_8x8DCT;
                di = 4;
            }

//                fill_intra4x4_pred_table(h);
            for(i=0; i<16; i+=di){
                int mode = pred_intra_mode(h, sl, i);

                if(!get_bits1(&sl->gb)){
                    const int rem_mode= get_bits(&sl->gb, 3);
                    mode = rem_mode + (rem_mode >= mode);
                }

                if(di==4)
                    fill_rectangle(&sl->intra4x4_pred_mode_cache[ scan8[i] ], 2, 2, 8, mode, 1);
                else
                    sl->intra4x4_pred_mode_cache[scan8[i]] = mode;
            }
            write_back_intra_pred_mode(h, sl);
            if (ff_h264_check_intra4x4_pred_mode(sl->intra4x4_pred_mode_cache, h->avctx,
                                                 sl->top_samples_available, sl->left_samples_available) < 0)
                return -1;
        }else{
            sl->intra16x16_pred_mode = ff_h264_check_intra_pred_mode(h->avctx, sl->top_samples_available,
                                                                     sl->left_samples_available, sl->intra16x16_pred_mode, 0);
            if (sl->intra16x16_pred_mode < 0)
                return -1;
        }
        if(decode_chroma){
            pred_mode= ff_h264_check_intra_pred_mode(h->avctx, sl->top_samples_available,
                                                     sl->left_samples_available, get_ue_golomb_31(&sl->gb), 1);
            if(pred_mode < 0)
                return -1;
            sl->chroma_pred_mode = pred_mode;
        } else {
            sl->chroma_pred_mode = DC_128_PRED8x8;
        }
    }else if(partition_count==4){
        int i, j, sub_partition_count[4], list, ref[2][4];

        if (sl->slice_type_nos == AV_PICTURE_TYPE_B) {
            for(i=0; i<4; i++){
                sl->sub_mb_type[i]= get_ue_golomb_31(&sl->gb);
                if(sl->sub_mb_type[i] >=13){
                    av_log(h->avctx, AV_LOG_ERROR, "B sub_mb_type %u out of range at %d %d\n", sl->sub_mb_type[i], sl->mb_x, sl->mb_y);
                    return -1;
                }
                sub_partition_count[i] = ff_h264_b_sub_mb_type_info[sl->sub_mb_type[i]].partition_count;
                sl->sub_mb_type[i]     = ff_h264_b_sub_mb_type_info[sl->sub_mb_type[i]].type;
            }
            if( IS_DIRECT(sl->sub_mb_type[0]|sl->sub_mb_type[1]|sl->sub_mb_type[2]|sl->sub_mb_type[3])) {
                ff_h264_pred_direct_motion(h, sl, &mb_type);
                sl->ref_cache[0][scan8[4]] =
                sl->ref_cache[1][scan8[4]] =
                sl->ref_cache[0][scan8[12]] =
                sl->ref_cache[1][scan8[12]] = PART_NOT_AVAILABLE;
            }
        }else{
            av_assert2(sl->slice_type_nos == AV_PICTURE_TYPE_P); //FIXME SP correct ?
            for(i=0; i<4; i++){
                sl->sub_mb_type[i]= get_ue_golomb_31(&sl->gb);
                if(sl->sub_mb_type[i] >=4){
                    av_log(h->avctx, AV_LOG_ERROR, "P sub_mb_type %u out of range at %d %d\n", sl->sub_mb_type[i], sl->mb_x, sl->mb_y);
                    return -1;
                }
                sub_partition_count[i] = ff_h264_p_sub_mb_type_info[sl->sub_mb_type[i]].partition_count;
                sl->sub_mb_type[i]     = ff_h264_p_sub_mb_type_info[sl->sub_mb_type[i]].type;
            }
        }

        for (list = 0; list < sl->list_count; list++) {
            int ref_count = IS_REF0(mb_type) ? 1 : sl->ref_count[list] << MB_MBAFF(sl);
            for(i=0; i<4; i++){
                if(IS_DIRECT(sl->sub_mb_type[i])) continue;
                if(IS_DIR(sl->sub_mb_type[i], 0, list)){
                    unsigned int tmp;
                    if(ref_count == 1){
                        tmp= 0;
                    }else if(ref_count == 2){
                        tmp= get_bits1(&sl->gb)^1;
                    }else{
                        tmp= get_ue_golomb_31(&sl->gb);
                        if(tmp>=ref_count){
                            av_log(h->avctx, AV_LOG_ERROR, "ref %u overflow\n", tmp);
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
            dct8x8_allowed = get_dct8x8_allowed(h, sl);

        for (list = 0; list < sl->list_count; list++) {
            for(i=0; i<4; i++){
                if(IS_DIRECT(sl->sub_mb_type[i])) {
                    sl->ref_cache[list][ scan8[4*i] ] = sl->ref_cache[list][ scan8[4*i]+1 ];
                    continue;
                }
                sl->ref_cache[list][ scan8[4*i]   ]=sl->ref_cache[list][ scan8[4*i]+1 ]=
                sl->ref_cache[list][ scan8[4*i]+8 ]=sl->ref_cache[list][ scan8[4*i]+9 ]= ref[list][i];

                if(IS_DIR(sl->sub_mb_type[i], 0, list)){
                    const int sub_mb_type= sl->sub_mb_type[i];
                    const int block_width= (sub_mb_type & (MB_TYPE_16x16|MB_TYPE_16x8)) ? 2 : 1;
                    for(j=0; j<sub_partition_count[i]; j++){
                        int mx, my;
                        const int index= 4*i + block_width*j;
                        int16_t (* mv_cache)[2]= &sl->mv_cache[list][ scan8[index] ];
                        pred_motion(h, sl, index, block_width, list, sl->ref_cache[list][ scan8[index] ], &mx, &my);
                        mx += get_se_golomb(&sl->gb);
                        my += get_se_golomb(&sl->gb);
                        ff_tlog(h->avctx, "final mv:%d %d\n", mx, my);

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
                    uint32_t *p= (uint32_t *)&sl->mv_cache[list][ scan8[4*i] ][0];
                    p[0] = p[1]=
                    p[8] = p[9]= 0;
                }
            }
        }
    }else if(IS_DIRECT(mb_type)){
        ff_h264_pred_direct_motion(h, sl, &mb_type);
        dct8x8_allowed &= h->ps.sps->direct_8x8_inference_flag;
    }else{
        int list, mx, my, i;
         //FIXME we should set ref_idx_l? to 0 if we use that later ...
        if(IS_16X16(mb_type)){
            for (list = 0; list < sl->list_count; list++) {
                    unsigned int val;
                    if(IS_DIR(mb_type, 0, list)){
                        unsigned rc = sl->ref_count[list] << MB_MBAFF(sl);
                        if (rc == 1) {
                            val= 0;
                        } else if (rc == 2) {
                            val= get_bits1(&sl->gb)^1;
                        }else{
                            val= get_ue_golomb_31(&sl->gb);
                            if (val >= rc) {
                                av_log(h->avctx, AV_LOG_ERROR, "ref %u overflow\n", val);
                                return -1;
                            }
                        }
                    fill_rectangle(&sl->ref_cache[list][ scan8[0] ], 4, 4, 8, val, 1);
                    }
            }
            for (list = 0; list < sl->list_count; list++) {
                if(IS_DIR(mb_type, 0, list)){
                    pred_motion(h, sl, 0, 4, list, sl->ref_cache[list][ scan8[0] ], &mx, &my);
                    mx += get_se_golomb(&sl->gb);
                    my += get_se_golomb(&sl->gb);
                    ff_tlog(h->avctx, "final mv:%d %d\n", mx, my);

                    fill_rectangle(sl->mv_cache[list][ scan8[0] ], 4, 4, 8, pack16to32(mx,my), 4);
                }
            }
        }
        else if(IS_16X8(mb_type)){
            for (list = 0; list < sl->list_count; list++) {
                    for(i=0; i<2; i++){
                        unsigned int val;
                        if(IS_DIR(mb_type, i, list)){
                            unsigned rc = sl->ref_count[list] << MB_MBAFF(sl);
                            if (rc == 1) {
                                val= 0;
                            } else if (rc == 2) {
                                val= get_bits1(&sl->gb)^1;
                            }else{
                                val= get_ue_golomb_31(&sl->gb);
                                if (val >= rc) {
                                    av_log(h->avctx, AV_LOG_ERROR, "ref %u overflow\n", val);
                                    return -1;
                                }
                            }
                        }else
                            val= LIST_NOT_USED&0xFF;
                        fill_rectangle(&sl->ref_cache[list][ scan8[0] + 16*i ], 4, 2, 8, val, 1);
                    }
            }
            for (list = 0; list < sl->list_count; list++) {
                for(i=0; i<2; i++){
                    unsigned int val;
                    if(IS_DIR(mb_type, i, list)){
                        pred_16x8_motion(h, sl, 8*i, list, sl->ref_cache[list][scan8[0] + 16*i], &mx, &my);
                        mx += get_se_golomb(&sl->gb);
                        my += get_se_golomb(&sl->gb);
                        ff_tlog(h->avctx, "final mv:%d %d\n", mx, my);

                        val= pack16to32(mx,my);
                    }else
                        val=0;
                    fill_rectangle(sl->mv_cache[list][ scan8[0] + 16*i ], 4, 2, 8, val, 4);
                }
            }
        }else{
            av_assert2(IS_8X16(mb_type));
            for (list = 0; list < sl->list_count; list++) {
                    for(i=0; i<2; i++){
                        unsigned int val;
                        if(IS_DIR(mb_type, i, list)){ //FIXME optimize
                            unsigned rc = sl->ref_count[list] << MB_MBAFF(sl);
                            if (rc == 1) {
                                val= 0;
                            } else if (rc == 2) {
                                val= get_bits1(&sl->gb)^1;
                            }else{
                                val= get_ue_golomb_31(&sl->gb);
                                if (val >= rc) {
                                    av_log(h->avctx, AV_LOG_ERROR, "ref %u overflow\n", val);
                                    return -1;
                                }
                            }
                        }else
                            val= LIST_NOT_USED&0xFF;
                        fill_rectangle(&sl->ref_cache[list][ scan8[0] + 2*i ], 2, 4, 8, val, 1);
                    }
            }
            for (list = 0; list < sl->list_count; list++) {
                for(i=0; i<2; i++){
                    unsigned int val;
                    if(IS_DIR(mb_type, i, list)){
                        pred_8x16_motion(h, sl, i*4, list, sl->ref_cache[list][ scan8[0] + 2*i ], &mx, &my);
                        mx += get_se_golomb(&sl->gb);
                        my += get_se_golomb(&sl->gb);
                        ff_tlog(h->avctx, "final mv:%d %d\n", mx, my);

                        val= pack16to32(mx,my);
                    }else
                        val=0;
                    fill_rectangle(sl->mv_cache[list][ scan8[0] + 2*i ], 2, 4, 8, val, 4);
                }
            }
        }
    }

    if(IS_INTER(mb_type))
        write_back_motion(h, sl, mb_type);

    if(!IS_INTRA16x16(mb_type)){
        cbp= get_ue_golomb(&sl->gb);

        if(decode_chroma){
            if(cbp > 47){
                av_log(h->avctx, AV_LOG_ERROR, "cbp too large (%u) at %d %d\n", cbp, sl->mb_x, sl->mb_y);
                return -1;
            }
            if (IS_INTRA4x4(mb_type))
                cbp = ff_h264_golomb_to_intra4x4_cbp[cbp];
            else
                cbp = ff_h264_golomb_to_inter_cbp[cbp];
        }else{
            if(cbp > 15){
                av_log(h->avctx, AV_LOG_ERROR, "cbp too large (%u) at %d %d\n", cbp, sl->mb_x, sl->mb_y);
                return -1;
            }
            if(IS_INTRA4x4(mb_type)) cbp= golomb_to_intra4x4_cbp_gray[cbp];
            else                     cbp= golomb_to_inter_cbp_gray[cbp];
        }
    } else {
        if (!decode_chroma && cbp>15) {
            av_log(h->avctx, AV_LOG_ERROR, "gray chroma\n");
            return AVERROR_INVALIDDATA;
        }
    }

    if(dct8x8_allowed && (cbp&15) && !IS_INTRA(mb_type)){
        mb_type |= MB_TYPE_8x8DCT*get_bits1(&sl->gb);
    }
    sl->cbp=
    h->cbp_table[mb_xy]= cbp;
    h->cur_pic.mb_type[mb_xy] = mb_type;

    if(cbp || IS_INTRA16x16(mb_type)){
        int i4x4, i8x8, chroma_idx;
        int dquant;
        int ret;
        GetBitContext *gb = &sl->gb;
        const uint8_t *scan, *scan8x8;
        const int max_qp = 51 + 6 * (h->ps.sps->bit_depth_luma - 8);

        if(IS_INTERLACED(mb_type)){
            scan8x8 = sl->qscale ? h->field_scan8x8_cavlc : h->field_scan8x8_cavlc_q0;
            scan    = sl->qscale ? h->field_scan : h->field_scan_q0;
        }else{
            scan8x8 = sl->qscale ? h->zigzag_scan8x8_cavlc : h->zigzag_scan8x8_cavlc_q0;
            scan    = sl->qscale ? h->zigzag_scan : h->zigzag_scan_q0;
        }

        dquant= get_se_golomb(&sl->gb);

        sl->qscale += dquant;

        if (((unsigned)sl->qscale) > max_qp){
            if (sl->qscale < 0) sl->qscale += max_qp + 1;
            else                sl->qscale -= max_qp+1;
            if (((unsigned)sl->qscale) > max_qp){
                av_log(h->avctx, AV_LOG_ERROR, "dquant out of range (%d) at %d %d\n", dquant, sl->mb_x, sl->mb_y);
                return -1;
            }
        }

        sl->chroma_qp[0] = get_chroma_qp(h->ps.pps, 0, sl->qscale);
        sl->chroma_qp[1] = get_chroma_qp(h->ps.pps, 1, sl->qscale);

        if ((ret = decode_luma_residual(h, sl, gb, scan, scan8x8, pixel_shift, mb_type, cbp, 0)) < 0 ) {
            return -1;
        }
        h->cbp_table[mb_xy] |= ret << 12;
        if (CHROMA444(h)) {
            if (decode_luma_residual(h, sl, gb, scan, scan8x8, pixel_shift, mb_type, cbp, 1) < 0 ) {
                return -1;
            }
            if (decode_luma_residual(h, sl, gb, scan, scan8x8, pixel_shift, mb_type, cbp, 2) < 0 ) {
                return -1;
            }
        } else {
            const int num_c8x8 = h->ps.sps->chroma_format_idc;

            if(cbp&0x30){
                for(chroma_idx=0; chroma_idx<2; chroma_idx++)
                    if (decode_residual(h, sl, gb, sl->mb + ((256 + 16*16*chroma_idx) << pixel_shift),
                                        CHROMA_DC_BLOCK_INDEX + chroma_idx,
                                        CHROMA422(h) ? ff_h264_chroma422_dc_scan : ff_h264_chroma_dc_scan,
                                        NULL, 4 * num_c8x8) < 0) {
                        return -1;
                    }
            }

            if(cbp&0x20){
                for(chroma_idx=0; chroma_idx<2; chroma_idx++){
                    const uint32_t *qmul = h->ps.pps->dequant4_coeff[chroma_idx+1+(IS_INTRA( mb_type ) ? 0:3)][sl->chroma_qp[chroma_idx]];
                    int16_t *mb = sl->mb + (16*(16 + 16*chroma_idx) << pixel_shift);
                    for (i8x8 = 0; i8x8<num_c8x8; i8x8++) {
                        for (i4x4 = 0; i4x4 < 4; i4x4++) {
                            const int index = 16 + 16*chroma_idx + 8*i8x8 + i4x4;
                            if (decode_residual(h, sl, gb, mb, index, scan + 1, qmul, 15) < 0)
                                return -1;
                            mb += 16 << pixel_shift;
                        }
                    }
                }
            }else{
                fill_rectangle(&sl->non_zero_count_cache[scan8[16]], 4, 4, 8, 0, 1);
                fill_rectangle(&sl->non_zero_count_cache[scan8[32]], 4, 4, 8, 0, 1);
            }
        }
    }else{
        fill_rectangle(&sl->non_zero_count_cache[scan8[ 0]], 4, 4, 8, 0, 1);
        fill_rectangle(&sl->non_zero_count_cache[scan8[16]], 4, 4, 8, 0, 1);
        fill_rectangle(&sl->non_zero_count_cache[scan8[32]], 4, 4, 8, 0, 1);
    }
    h->cur_pic.qscale_table[mb_xy] = sl->qscale;
    write_back_non_zero_count(h, sl);

    return 0;
}
