static unsigned int mszh_decomp(unsigned char * srcptr, int srclen, unsigned char * destptr, unsigned int destsize)
{
    unsigned char *destptr_bak = destptr;
    unsigned char *destptr_end = destptr + destsize;
    unsigned char mask = 0;
    unsigned char maskbit = 0;
    unsigned int ofs, cnt;

    while (srclen > 0 && destptr < destptr_end) {
        if (maskbit == 0) {
            mask = *srcptr++;
            maskbit = 8;
            srclen--;
            continue;
        }
        if ((mask & (1 << (--maskbit))) == 0) {
            if (destptr + 4 > destptr_end)
                break;
            memcpy(destptr, srcptr, 4);
            srclen -= 4;
            destptr += 4;
            srcptr += 4;
        } else {
            ofs = *srcptr++;
            cnt = *srcptr++;
            ofs += cnt * 256;
            cnt = ((cnt >> 3) & 0x1f) + 1;
            ofs &= 0x7ff;
            srclen -= 2;
            cnt *= 4;
            if (destptr + cnt > destptr_end) {
                cnt =  destptr_end - destptr;
            }
            for (; cnt > 0; cnt--) {
                *destptr = *(destptr - ofs);
                destptr++;
            }
        }
    }

    return destptr - destptr_bak;
}
