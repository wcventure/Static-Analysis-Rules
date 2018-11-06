int h263_decode_mb(MpegEncContext *s,
                   DCTELEM block[6][64])
{
    int cbpc, cbpy, i, cbp, pred_x, pred_y, mx, my, dquant;
    INT16 *mot_val;
    static UINT8 quant_tab[4] = { -1, -2, 1, 2 };

    if (s->pict_type == P_TYPE) {
        if (get_bits1(&s->gb)) {
            /
            s->mb_intra = 0;
            for(i=0;i<6;i++)
                s->block_last_index[i] = -1;
            s->mv_dir = MV_DIR_FORWARD;
            s->mv_type = MV_TYPE_16X16;
            s->mv[0][0][0] = 0;
            s->mv[0][0][1] = 0;
            s->mb_skiped = 1;
            return 0;
        }
        cbpc = get_vlc(&s->gb, &inter_MCBPC_vlc);
        if (cbpc < 0)
            return -1;
        dquant = cbpc & 8;
        s->mb_intra = ((cbpc & 4) != 0);
    } else {
        cbpc = get_vlc(&s->gb, &intra_MCBPC_vlc);
        if (cbpc < 0)
            return -1;
        dquant = cbpc & 4;
        s->mb_intra = 1;
    }

    if (!s->mb_intra) {
        cbpy = get_vlc(&s->gb, &cbpy_vlc);
        cbp = (cbpc & 3) | ((cbpy ^ 0xf) << 2);
        if (dquant) {
            s->qscale += quant_tab[get_bits(&s->gb, 2)];
            if (s->qscale < 1)
                s->qscale = 1;
            else if (s->qscale > 31)
                s->qscale = 31;
        }
        s->mv_dir = MV_DIR_FORWARD;
        if ((cbpc & 16) == 0) {
            /
            s->mv_type = MV_TYPE_16X16;
            h263_pred_motion(s, 0, &pred_x, &pred_y);
            if (umvplus_dec)
               mx = h263p_decode_umotion(s, pred_x);
            else
               mx = h263_decode_motion(s, pred_x);
            if (mx >= 0xffff)
                return -1;
            
            if (umvplus_dec)
               my = h263p_decode_umotion(s, pred_y);
            else    
               my = h263_decode_motion(s, pred_y);
            if (my >= 0xffff)
                return -1;
            s->mv[0][0][0] = mx;
            s->mv[0][0][1] = my;
            if (umvplus_dec && (mx - pred_x) == 1 && (my - pred_y) == 1)
               skip_bits1(&s->gb); /
                           
        } else {
            s->mv_type = MV_TYPE_8X8;
            for(i=0;i<4;i++) {
                mot_val = h263_pred_motion(s, i, &pred_x, &pred_y);
                if (umvplus_dec)
                  mx = h263p_decode_umotion(s, pred_x);
                else
                  mx = h263_decode_motion(s, pred_x);
                if (mx >= 0xffff)
                    return -1;
                
                if (umvplus_dec)
                  my = h263p_decode_umotion(s, pred_y);
                else    
                  my = h263_decode_motion(s, pred_y);
                if (my >= 0xffff)
                    return -1;
                s->mv[0][i][0] = mx;
                s->mv[0][i][1] = my;
                if (umvplus_dec && (mx - pred_x) == 1 && (my - pred_y) == 1)
                  skip_bits1(&s->gb); /
                mot_val[0] = mx;
                mot_val[1] = my;
            }
        }
    } else {
        s->ac_pred = 0;
	if (s->h263_pred) {
            s->ac_pred = get_bits1(&s->gb);
        }
        cbpy = get_vlc(&s->gb, &cbpy_vlc);
        cbp = (cbpc & 3) | (cbpy << 2);
        if (dquant) {
            s->qscale += quant_tab[get_bits(&s->gb, 2)];
            if (s->qscale < 1)
                s->qscale = 1;
            else if (s->qscale > 31)
                s->qscale = 31;
        }
    }

    /
    if (s->h263_pred) {
	for (i = 0; i < 6; i++) {
	    if (mpeg4_decode_block(s, block[i], i, (cbp >> (5 - i)) & 1) < 0)
                return -1;
	}
    } else {
	for (i = 0; i < 6; i++) {
	    if (h263_decode_block(s, block[i], i, (cbp >> (5 - i)) & 1) < 0)
                return -1;
	}
    }
    return 0;
}
