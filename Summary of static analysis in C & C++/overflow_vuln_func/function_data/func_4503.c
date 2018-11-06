static void xa_decode(short *out, const unsigned char *in,
    ADPCMChannelStatus *left, ADPCMChannelStatus *right, int inc)
{
    int i, j;
    int shift,filter,f0,f1;
    int s_1,s_2;
    int d,s,t;

    for(i=0;i<4;i++) {

        shift  = 12 - (in[4+i*2] & 15);
        filter = in[4+i*2] >> 4;
        f0 = xa_adpcm_table[filter][0];
        f1 = xa_adpcm_table[filter][1];

        s_1 = left->sample1;
        s_2 = left->sample2;

        for(j=0;j<28;j++) {
            d = in[16+i+j*4];

            t = (signed char)(d<<4)>>4;
            s = ( t<<shift ) + ((s_1*f0 + s_2*f1+32)>>6);
            s_2 = s_1;
            s_1 = av_clip_int16(s);
            *out = s_1;
            out += inc;
        }

        if (inc==2) { /
            left->sample1 = s_1;
            left->sample2 = s_2;
            s_1 = right->sample1;
            s_2 = right->sample2;
            out = out + 1 - 28*2;
        }

        shift  = 12 - (in[5+i*2] & 15);
        filter = in[5+i*2] >> 4;

        f0 = xa_adpcm_table[filter][0];
        f1 = xa_adpcm_table[filter][1];

        for(j=0;j<28;j++) {
            d = in[16+i+j*4];

            t = (signed char)d >> 4;
            s = ( t<<shift ) + ((s_1*f0 + s_2*f1+32)>>6);
            s_2 = s_1;
            s_1 = av_clip_int16(s);
            *out = s_1;
            out += inc;
        }

        if (inc==2) { /
            right->sample1 = s_1;
            right->sample2 = s_2;
            out -= 1;
        } else {
            left->sample1 = s_1;
            left->sample2 = s_2;
        }
    }
}
