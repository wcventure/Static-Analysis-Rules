int h263_decode_picture_header(MpegEncContext *s)
{
    int format, width, height;

    /
    if (get_bits(&s->gb, 22) != 0x20)
        return -1;
    skip_bits(&s->gb, 8); /

    if (get_bits1(&s->gb) != 1)
        return -1;	/
    if (get_bits1(&s->gb) != 0)
        return -1;	/
    skip_bits1(&s->gb);	/
    skip_bits1(&s->gb);	/
    skip_bits1(&s->gb);	/

    format = get_bits(&s->gb, 3);

    if (format != 7) {
        s->h263_plus = 0;
        /
        width = h263_format[format][0];
        height = h263_format[format][1];
        if (!width)
            return -1;

        s->pict_type = I_TYPE + get_bits1(&s->gb);

        s->unrestricted_mv = get_bits1(&s->gb); 
        s->h263_long_vectors = s->unrestricted_mv;

        if (get_bits1(&s->gb) != 0)
            return -1;	/
        if (get_bits1(&s->gb) != 0)
            return -1;	/
        if (get_bits1(&s->gb) != 0)
            return -1;	/

        s->qscale = get_bits(&s->gb, 5);
        skip_bits1(&s->gb);	/
    } else {
        s->h263_plus = 1;
        /
        /
     
        if (get_bits(&s->gb, 3) != 1) /
            return -1;
        format = get_bits(&s->gb, 3);
                
        skip_bits(&s->gb,1); /
        umvplus_dec = get_bits(&s->gb, 1); /
        skip_bits(&s->gb, 10);
        skip_bits(&s->gb, 3); /
        
        /
        s->pict_type = get_bits(&s->gb, 3) + 1;
        if (s->pict_type != I_TYPE &&
            s->pict_type != P_TYPE)
            return -1;
        skip_bits(&s->gb, 7);
        
        /
        if (format == 6) {
            /
            skip_bits(&s->gb, 4); /
            width = (get_bits(&s->gb, 9) + 1) * 4;
            skip_bits1(&s->gb);
            height = get_bits(&s->gb, 9) * 4;
#ifdef DEBUG 
            fprintf(stderr,"\nH.263+ Custom picture: %dx%d\n",width,height);
#endif            
        }
        else {
            width = h263_format[format][0];
            height = h263_format[format][1];
        }
        
        if ((width == 0) || (height == 0))
            return -1;
            
        if (umvplus_dec) {
            skip_bits1(&s->gb); /
        }
            
        s->qscale = get_bits(&s->gb, 5);
    }
    /
    while (get_bits1(&s->gb) != 0) {
        skip_bits(&s->gb, 8);
    }
    s->f_code = 1;
    s->width = width;
    s->height = height;
    return 0;
}
