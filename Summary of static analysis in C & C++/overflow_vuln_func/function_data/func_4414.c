static void rv40_loop_filter(RV34DecContext *r, int row)
{
    MpegEncContext *s = &r->s;
    int mb_pos, mb_x;
    int i, j, k;
    uint8_t *Y, *C;
    int alpha, beta, betaY, betaC;
    int q;
    int mbtype[4];   ///< current macroblock and its neighbours types
    /
    int mb_strong[4];
    int clip[4];     ///< MB filter clipping value calculated from filtering strength
    /
    int cbp[4];
    /
    int uvcbp[4][2];
    /
    int mvmasks[4];

    mb_pos = row * s->mb_stride;
    for(mb_x = 0; mb_x < s->mb_width; mb_x++, mb_pos++){
        int mbtype = s->current_picture_ptr->f.mb_type[mb_pos];
        if(IS_INTRA(mbtype) || IS_SEPARATE_DC(mbtype))
            r->cbp_luma  [mb_pos] = r->deblock_coefs[mb_pos] = 0xFFFF;
        if(IS_INTRA(mbtype))
            r->cbp_chroma[mb_pos] = 0xFF;
    }
    mb_pos = row * s->mb_stride;
    for(mb_x = 0; mb_x < s->mb_width; mb_x++, mb_pos++){
        int y_h_deblock, y_v_deblock;
        int c_v_deblock[2], c_h_deblock[2];
        int clip_left;
        int avail[4];
        int y_to_deblock, c_to_deblock[2];

        q = s->current_picture_ptr->f.qscale_table[mb_pos];
        alpha = rv40_alpha_tab[q];
        beta  = rv40_beta_tab [q];
        betaY = betaC = beta * 3;
        if(s->width * s->height <= 176*144)
            betaY += beta;

        avail[0] = 1;
        avail[1] = row;
        avail[2] = mb_x;
        avail[3] = row < s->mb_height - 1;
        for(i = 0; i < 4; i++){
            if(avail[i]){
                int pos = mb_pos + neighbour_offs_x[i] + neighbour_offs_y[i]*s->mb_stride;
                mvmasks[i] = r->deblock_coefs[pos];
                mbtype [i] = s->current_picture_ptr->f.mb_type[pos];
                cbp    [i] = r->cbp_luma[pos];
                uvcbp[i][0] = r->cbp_chroma[pos] & 0xF;
                uvcbp[i][1] = r->cbp_chroma[pos] >> 4;
            }else{
                mvmasks[i] = 0;
                mbtype [i] = mbtype[0];
                cbp    [i] = 0;
                uvcbp[i][0] = uvcbp[i][1] = 0;
            }
            mb_strong[i] = IS_INTRA(mbtype[i]) || IS_SEPARATE_DC(mbtype[i]);
            clip[i] = rv40_filter_clip_tbl[mb_strong[i] + 1][q];
        }
        y_to_deblock =  mvmasks[POS_CUR]
                     | (mvmasks[POS_BOTTOM] << 16);
        /
        y_h_deblock =   y_to_deblock
                    | ((cbp[POS_CUR]                           <<  4) & ~MASK_Y_TOP_ROW)
                    | ((cbp[POS_TOP]        & MASK_Y_LAST_ROW) >> 12);
        /
        y_v_deblock =   y_to_deblock
                    | ((cbp[POS_CUR]                      << 1) & ~MASK_Y_LEFT_COL)
                    | ((cbp[POS_LEFT] & MASK_Y_RIGHT_COL) >> 3);
        if(!mb_x)
            y_v_deblock &= ~MASK_Y_LEFT_COL;
        if(!row)
            y_h_deblock &= ~MASK_Y_TOP_ROW;
        if(row == s->mb_height - 1 || (mb_strong[POS_CUR] || mb_strong[POS_BOTTOM]))
            y_h_deblock &= ~(MASK_Y_TOP_ROW << 16);
        /
        for(i = 0; i < 2; i++){
            c_to_deblock[i] = (uvcbp[POS_BOTTOM][i] << 4) | uvcbp[POS_CUR][i];
            c_v_deblock[i] =   c_to_deblock[i]
                           | ((uvcbp[POS_CUR] [i]                       << 1) & ~MASK_C_LEFT_COL)
                           | ((uvcbp[POS_LEFT][i]   & MASK_C_RIGHT_COL) >> 1);
            c_h_deblock[i] =   c_to_deblock[i]
                           | ((uvcbp[POS_TOP][i]    & MASK_C_LAST_ROW)  >> 2)
                           |  (uvcbp[POS_CUR][i]                        << 2);
            if(!mb_x)
                c_v_deblock[i] &= ~MASK_C_LEFT_COL;
            if(!row)
                c_h_deblock[i] &= ~MASK_C_TOP_ROW;
            if(row == s->mb_height - 1 || mb_strong[POS_CUR] || mb_strong[POS_BOTTOM])
                c_h_deblock[i] &= ~(MASK_C_TOP_ROW << 4);
        }

        for(j = 0; j < 16; j += 4){
            Y = s->current_picture_ptr->f.data[0] + mb_x*16 + (row*16 + j) * s->linesize;
            for(i = 0; i < 4; i++, Y += 4){
                int ij = i + j;
                int clip_cur = y_to_deblock & (MASK_CUR << ij) ? clip[POS_CUR] : 0;
                int dither = j ? ij : i*4;

                // if bottom block is coded then we can filter its top edge
                // (or bottom edge of this block, which is the same)
                if(y_h_deblock & (MASK_BOTTOM << ij)){
                    rv40_adaptive_loop_filter(&r->rdsp, Y+4*s->linesize,
                                              s->linesize, dither,
                                              y_to_deblock & (MASK_BOTTOM << ij) ? clip[POS_CUR] : 0,
                                              clip_cur, alpha, beta, betaY,
                                              0, 0, 0);
                }
                // filter left block edge in ordinary mode (with low filtering strength)
                if(y_v_deblock & (MASK_CUR << ij) && (i || !(mb_strong[POS_CUR] || mb_strong[POS_LEFT]))){
                    if(!i)
                        clip_left = mvmasks[POS_LEFT] & (MASK_RIGHT << j) ? clip[POS_LEFT] : 0;
                    else
                        clip_left = y_to_deblock & (MASK_CUR << (ij-1)) ? clip[POS_CUR] : 0;
                    rv40_adaptive_loop_filter(&r->rdsp, Y, s->linesize, dither,
                                              clip_cur,
                                              clip_left,
                                              alpha, beta, betaY, 0, 0, 1);
                }
                // filter top edge of the current macroblock when filtering strength is high
                if(!j && y_h_deblock & (MASK_CUR << i) && (mb_strong[POS_CUR] || mb_strong[POS_TOP])){
                    rv40_adaptive_loop_filter(&r->rdsp, Y, s->linesize, dither,
                                       clip_cur,
                                       mvmasks[POS_TOP] & (MASK_TOP << i) ? clip[POS_TOP] : 0,
                                       alpha, beta, betaY, 0, 1, 0);
                }
                // filter left block edge in edge mode (with high filtering strength)
                if(y_v_deblock & (MASK_CUR << ij) && !i && (mb_strong[POS_CUR] || mb_strong[POS_LEFT])){
                    clip_left = mvmasks[POS_LEFT] & (MASK_RIGHT << j) ? clip[POS_LEFT] : 0;
                    rv40_adaptive_loop_filter(&r->rdsp, Y, s->linesize, dither,
                                       clip_cur,
                                       clip_left,
                                       alpha, beta, betaY, 0, 1, 1);
                }
            }
        }
        for(k = 0; k < 2; k++){
            for(j = 0; j < 2; j++){
                C = s->current_picture_ptr->f.data[k + 1] + mb_x*8 + (row*8 + j*4) * s->uvlinesize;
                for(i = 0; i < 2; i++, C += 4){
                    int ij = i + j*2;
                    int clip_cur = c_to_deblock[k] & (MASK_CUR << ij) ? clip[POS_CUR] : 0;
                    if(c_h_deblock[k] & (MASK_CUR << (ij+2))){
                        int clip_bot = c_to_deblock[k] & (MASK_CUR << (ij+2)) ? clip[POS_CUR] : 0;
                        rv40_adaptive_loop_filter(&r->rdsp, C+4*s->uvlinesize, s->uvlinesize, i*8,
                                           clip_bot,
                                           clip_cur,
                                           alpha, beta, betaC, 1, 0, 0);
                    }
                    if((c_v_deblock[k] & (MASK_CUR << ij)) && (i || !(mb_strong[POS_CUR] || mb_strong[POS_LEFT]))){
                        if(!i)
                            clip_left = uvcbp[POS_LEFT][k] & (MASK_CUR << (2*j+1)) ? clip[POS_LEFT] : 0;
                        else
                            clip_left = c_to_deblock[k]    & (MASK_CUR << (ij-1))  ? clip[POS_CUR]  : 0;
                        rv40_adaptive_loop_filter(&r->rdsp, C, s->uvlinesize, j*8,
                                           clip_cur,
                                           clip_left,
                                           alpha, beta, betaC, 1, 0, 1);
                    }
                    if(!j && c_h_deblock[k] & (MASK_CUR << ij) && (mb_strong[POS_CUR] || mb_strong[POS_TOP])){
                        int clip_top = uvcbp[POS_TOP][k] & (MASK_CUR << (ij+2)) ? clip[POS_TOP] : 0;
                        rv40_adaptive_loop_filter(&r->rdsp, C, s->uvlinesize, i*8,
                                           clip_cur,
                                           clip_top,
                                           alpha, beta, betaC, 1, 1, 0);
                    }
                    if(c_v_deblock[k] & (MASK_CUR << ij) && !i && (mb_strong[POS_CUR] || mb_strong[POS_LEFT])){
                        clip_left = uvcbp[POS_LEFT][k] & (MASK_CUR << (2*j+1)) ? clip[POS_LEFT] : 0;
                        rv40_adaptive_loop_filter(&r->rdsp, C, s->uvlinesize, j*8,
                                           clip_cur,
                                           clip_left,
                                           alpha, beta, betaC, 1, 1, 1);
                    }
                }
            }
        }
    }
}
