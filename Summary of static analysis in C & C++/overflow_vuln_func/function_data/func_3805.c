void h263_encode_picture_header(MpegEncContext * s, int picture_number)
{
    int format;

    align_put_bits(&s->pb);

    /
    s->ptr_lastgob = pbBufPtr(&s->pb);
    s->gob_number = 0;

    put_bits(&s->pb, 22, 0x20); /
    put_bits(&s->pb, 8, ((s->picture_number * 30 * FRAME_RATE_BASE) / 
                         s->frame_rate) & 0xff);

    put_bits(&s->pb, 1, 1);	/
    put_bits(&s->pb, 1, 0);	/
    put_bits(&s->pb, 1, 0);	/
    put_bits(&s->pb, 1, 0);	/
    put_bits(&s->pb, 1, 0);	/
    
    format = h263_get_picture_format(s->width, s->height);
    if (!s->h263_plus) {
        /
        put_bits(&s->pb, 3, format);
        put_bits(&s->pb, 1, (s->pict_type == P_TYPE));
        /
        put_bits(&s->pb, 1, 0);	/
        put_bits(&s->pb, 1, 0);	/
        put_bits(&s->pb, 1, 0);	/
        put_bits(&s->pb, 1, 0);	/
        put_bits(&s->pb, 5, s->qscale);
        put_bits(&s->pb, 1, 0);	/
    } else {
        /
        /
        put_bits(&s->pb, 3, 7);
        put_bits(&s->pb,3,1); /
        if (format == 7)
            put_bits(&s->pb,3,6); /
        else
            put_bits(&s->pb, 3, format);
            
        put_bits(&s->pb,1,0); /
        s->umvplus = (s->pict_type == P_TYPE) && s->unrestricted_mv;
        put_bits(&s->pb, 1, s->umvplus); /
        put_bits(&s->pb,1,0); /
        put_bits(&s->pb,1,0); /
        put_bits(&s->pb,1,0); /
        put_bits(&s->pb,1,0); /
        put_bits(&s->pb,1,0); /
        put_bits(&s->pb,1,0); /
        put_bits(&s->pb,1,0); /
        put_bits(&s->pb,1,0); /
        put_bits(&s->pb,1,0); /
        put_bits(&s->pb,1,1); /
        put_bits(&s->pb,3,0); /
		
        put_bits(&s->pb, 3, s->pict_type == P_TYPE);
		
        put_bits(&s->pb,1,0); /
        put_bits(&s->pb,1,0); /
        put_bits(&s->pb,1,0); /
        put_bits(&s->pb,2,0); /
        put_bits(&s->pb,1,1); /
		
        /
        put_bits(&s->pb, 1, 0);	/
		
		if (format == 7) {
            /
		
            put_bits(&s->pb,4,2); /
            put_bits(&s->pb,9,(s->width >> 2) - 1);
            put_bits(&s->pb,1,1); /
            put_bits(&s->pb,9,(s->height >> 2));
        }
        
        /
        if (s->umvplus)
            put_bits(&s->pb,1,1); /
        put_bits(&s->pb, 5, s->qscale);
    }

    put_bits(&s->pb, 1, 0);	/
}
