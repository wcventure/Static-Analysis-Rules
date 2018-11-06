void ff_write_pass1_stats(MpegEncContext *s)
{
    snprintf(s->avctx->stats_out, 256,
             "in:%d out:%d type:%d q:%d itex:%d ptex:%d mv:%d misc:%d "
             "fcode:%d bcode:%d mc-var:%d var:%d icount:%d skipcount:%d hbits:%d;\n",
             s->current_picture_ptr->f.display_picture_number,
             s->current_picture_ptr->f.coded_picture_number,
             s->pict_type,
             s->current_picture.f.quality,
             s->i_tex_bits,
             s->p_tex_bits,
             s->mv_bits,
             s->misc_bits,
             s->f_code,
             s->b_code,
             s->current_picture.mc_mb_var_sum,
             s->current_picture.mb_var_sum,
             s->i_count, s->skip_count,
             s->header_bits);
}
