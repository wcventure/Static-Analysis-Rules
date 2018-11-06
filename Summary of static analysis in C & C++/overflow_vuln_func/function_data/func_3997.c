static int dvbsub_probe(AVProbeData *p)
{
    int i, j, k;
    const uint8_t *end = p->buf + p->buf_size;
    int type, len;
    int max_score = 0;

    for(i=0; i<p->buf_size; i++){
        if (p->buf[i] == 0x0f) {
            const uint8_t *ptr = p->buf + i;
            uint8_t histogram[6] = {0};
            int min = 255;
            for(j=0; ptr + 6 < end; j++) {
                if (*ptr != 0x0f)
                    break;
                type    = ptr[1];
                //page_id = AV_RB16(ptr + 2);
                len     = AV_RB16(ptr + 4);
                if (type == 0x80) {
                    ;
                } else if (type >= 0x10 && type <= 0x14) {
                    histogram[type - 0x10] ++;
                } else
                    break;
                ptr += 6 + len;
            }
            for (k=0; k < 4; k++) {
                min = FFMIN(min, histogram[k]);
            }
            if (min && j > max_score)
                max_score = j;
        }
    }

    if (max_score > 5)
        return AVPROBE_SCORE_EXTENSION;

    return 0;
}
