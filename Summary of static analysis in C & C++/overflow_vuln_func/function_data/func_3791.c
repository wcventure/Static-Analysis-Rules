static int mb_var_thread(AVCodecContext *c, void *arg){
    MpegEncContext *s= *(void**)arg;
    int mb_x, mb_y;

    ff_check_alignment();

    for(mb_y=s->start_mb_y; mb_y < s->end_mb_y; mb_y++) {
        for(mb_x=0; mb_x < s->mb_width; mb_x++) {
            int xx = mb_x * 16;
            int yy = mb_y * 16;
            uint8_t *pix = s->new_picture.f.data[0] + (yy * s->linesize) + xx;
            int varc;
            int sum = s->dsp.pix_sum(pix, s->linesize);

            varc = (s->dsp.pix_norm1(pix, s->linesize) - (((unsigned)(sum*sum))>>8) + 500 + 128)>>8;

            s->current_picture.mb_var [s->mb_stride * mb_y + mb_x] = varc;
            s->current_picture.mb_mean[s->mb_stride * mb_y + mb_x] = (sum+128)>>8;
            s->me.mb_var_sum_temp    += varc;
        }
    }
    return 0;
}
