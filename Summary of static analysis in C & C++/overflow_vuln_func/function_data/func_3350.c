void h263_encode_mb(MpegEncContext * s,
		    DCTELEM block[6][64],
		    int motion_x, int motion_y)
{
    int cbpc, cbpy, i, cbp, pred_x, pred_y;

    //    printf("**mb x=%d y=%d\n", s->mb_x, s->mb_y);
   if (!s->mb_intra) {
	   /
	   cbp = 0;
	   for (i = 0; i < 6; i++) {
	      if (s->block_last_index[i] >= 0)
		   cbp |= 1 << (5 - i);
	   }
	   if ((cbp | motion_x | motion_y) == 0) {
	      /
	      put_bits(&s->pb, 1, 1);
	      return;
	   }
	   put_bits(&s->pb, 1, 0);	/
	   cbpc = cbp & 3;
	   put_bits(&s->pb,
		inter_MCBPC_bits[cbpc],
		inter_MCBPC_code[cbpc]);
	   cbpy = cbp >> 2;
	   cbpy ^= 0xf;
	   put_bits(&s->pb, cbpy_tab[cbpy][1], cbpy_tab[cbpy][0]);

	   /
      h263_pred_motion(s, 0, &pred_x, &pred_y);
      
      if (!umvplus) {  
         h263_encode_motion(s, motion_x - pred_x);
         h263_encode_motion(s, motion_y - pred_y);
      }
      else {
         h263p_encode_umotion(s, motion_x - pred_x);
         h263p_encode_umotion(s, motion_y - pred_y);
         if (((motion_x - pred_x) == 1) && ((motion_y - pred_y) == 1))
            /
            put_bits(&s->pb,1,1);
      }
   } else {
	/
	cbp = 0;
	for (i = 0; i < 6; i++) {
	    if (s->block_last_index[i] >= 1)
		cbp |= 1 << (5 - i);
	}

	cbpc = cbp & 3;
	if (s->pict_type == I_TYPE) {
	    put_bits(&s->pb,
		     intra_MCBPC_bits[cbpc],
		     intra_MCBPC_code[cbpc]);
	} else {
	    put_bits(&s->pb, 1, 0);	/
	    put_bits(&s->pb,
		     inter_MCBPC_bits[cbpc + 4],
		     inter_MCBPC_code[cbpc + 4]);
	}
	if (s->h263_pred) {
	    /
	    put_bits(&s->pb, 1, 0);	/
	}
	cbpy = cbp >> 2;
	put_bits(&s->pb, cbpy_tab[cbpy][1], cbpy_tab[cbpy][0]);
    }

    /
    if (s->h263_pred) {
	for (i = 0; i < 6; i++) {
	    mpeg4_encode_block(s, block[i], i);
	}
    } else {
	for (i = 0; i < 6; i++) {
	    h263_encode_block(s, block[i], i);
	}
    }
}
