static void fft_calc_c(FFTContext *s, FFTComplex *z) {

    int nbits, i, n, num_transforms, offset, step;
    int n4, n2, n34;
    FFTSample tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;
    FFTComplex *tmpz;
    const int fft_size = (1 << s->nbits);
    int64_t accu;

    num_transforms = (0x2aab >> (16 - s->nbits)) | 1;

    for (n=0; n<num_transforms; n++){
        offset = ff_fft_offsets_lut[n] << 2;
        tmpz = z + offset;

        tmp1 = tmpz[0].re + tmpz[1].re;
        tmp5 = tmpz[2].re + tmpz[3].re;
        tmp2 = tmpz[0].im + tmpz[1].im;
        tmp6 = tmpz[2].im + tmpz[3].im;
        tmp3 = tmpz[0].re - tmpz[1].re;
        tmp8 = tmpz[2].im - tmpz[3].im;
        tmp4 = tmpz[0].im - tmpz[1].im;
        tmp7 = tmpz[2].re - tmpz[3].re;

        tmpz[0].re = tmp1 + tmp5;
        tmpz[2].re = tmp1 - tmp5;
        tmpz[0].im = tmp2 + tmp6;
        tmpz[2].im = tmp2 - tmp6;
        tmpz[1].re = tmp3 + tmp8;
        tmpz[3].re = tmp3 - tmp8;
        tmpz[1].im = tmp4 - tmp7;
        tmpz[3].im = tmp4 + tmp7;
    }

    if (fft_size < 8)
        return;

    num_transforms = (num_transforms >> 1) | 1;

    for (n=0; n<num_transforms; n++){
        offset = ff_fft_offsets_lut[n] << 3;
        tmpz = z + offset;

        tmp1 = tmpz[4].re + tmpz[5].re;
        tmp3 = tmpz[6].re + tmpz[7].re;
        tmp2 = tmpz[4].im + tmpz[5].im;
        tmp4 = tmpz[6].im + tmpz[7].im;
        tmp5 = tmp1 + tmp3;
        tmp7 = tmp1 - tmp3;
        tmp6 = tmp2 + tmp4;
        tmp8 = tmp2 - tmp4;

        tmp1 = tmpz[4].re - tmpz[5].re;
        tmp2 = tmpz[4].im - tmpz[5].im;
        tmp3 = tmpz[6].re - tmpz[7].re;
        tmp4 = tmpz[6].im - tmpz[7].im;

        tmpz[4].re = tmpz[0].re - tmp5;
        tmpz[0].re = tmpz[0].re + tmp5;
        tmpz[4].im = tmpz[0].im - tmp6;
        tmpz[0].im = tmpz[0].im + tmp6;
        tmpz[6].re = tmpz[2].re - tmp8;
        tmpz[2].re = tmpz[2].re + tmp8;
        tmpz[6].im = tmpz[2].im + tmp7;
        tmpz[2].im = tmpz[2].im - tmp7;

        accu = (int64_t)Q31(M_SQRT1_2)*(tmp1 + tmp2);
        tmp5 = (int32_t)((accu + 0x40000000) >> 31);
        accu = (int64_t)Q31(M_SQRT1_2)*(tmp3 - tmp4);
        tmp7 = (int32_t)((accu + 0x40000000) >> 31);
        accu = (int64_t)Q31(M_SQRT1_2)*(tmp2 - tmp1);
        tmp6 = (int32_t)((accu + 0x40000000) >> 31);
        accu = (int64_t)Q31(M_SQRT1_2)*(tmp3 + tmp4);
        tmp8 = (int32_t)((accu + 0x40000000) >> 31);
        tmp1 = tmp5 + tmp7;
        tmp3 = tmp5 - tmp7;
        tmp2 = tmp6 + tmp8;
        tmp4 = tmp6 - tmp8;

        tmpz[5].re = tmpz[1].re - tmp1;
        tmpz[1].re = tmpz[1].re + tmp1;
        tmpz[5].im = tmpz[1].im - tmp2;
        tmpz[1].im = tmpz[1].im + tmp2;
        tmpz[7].re = tmpz[3].re - tmp4;
        tmpz[3].re = tmpz[3].re + tmp4;
        tmpz[7].im = tmpz[3].im + tmp3;
        tmpz[3].im = tmpz[3].im - tmp3;
    }

    step = 1 << ((MAX_LOG2_NFFT-4) - 4);
    n4 = 4;

    for (nbits=4; nbits<=s->nbits; nbits++){
        n2  = 2*n4;
        n34 = 3*n4;
        num_transforms = (num_transforms >> 1) | 1;

        for (n=0; n<num_transforms; n++){
            const FFTSample *w_re_ptr = ff_w_tab_sr + step;
            const FFTSample *w_im_ptr = ff_w_tab_sr + MAX_FFT_SIZE/(4*16) - step;
            offset = ff_fft_offsets_lut[n] << nbits;
            tmpz = z + offset;

            tmp5 = tmpz[ n2].re + tmpz[n34].re;
            tmp1 = tmpz[ n2].re - tmpz[n34].re;
            tmp6 = tmpz[ n2].im + tmpz[n34].im;
            tmp2 = tmpz[ n2].im - tmpz[n34].im;

            tmpz[ n2].re = tmpz[ 0].re - tmp5;
            tmpz[  0].re = tmpz[ 0].re + tmp5;
            tmpz[ n2].im = tmpz[ 0].im - tmp6;
            tmpz[  0].im = tmpz[ 0].im + tmp6;
            tmpz[n34].re = tmpz[n4].re - tmp2;
            tmpz[ n4].re = tmpz[n4].re + tmp2;
            tmpz[n34].im = tmpz[n4].im + tmp1;
            tmpz[ n4].im = tmpz[n4].im - tmp1;

            for (i=1; i<n4; i++){
                FFTSample w_re = w_re_ptr[0];
                FFTSample w_im = w_im_ptr[0];
                accu  = (int64_t)w_re*tmpz[ n2+i].re;
                accu += (int64_t)w_im*tmpz[ n2+i].im;
                tmp1 = (int32_t)((accu + 0x40000000) >> 31);
                accu  = (int64_t)w_re*tmpz[ n2+i].im;
                accu -= (int64_t)w_im*tmpz[ n2+i].re;
                tmp2 = (int32_t)((accu + 0x40000000) >> 31);
                accu  = (int64_t)w_re*tmpz[n34+i].re;
                accu -= (int64_t)w_im*tmpz[n34+i].im;
                tmp3 = (int32_t)((accu + 0x40000000) >> 31);
                accu  = (int64_t)w_re*tmpz[n34+i].im;
                accu += (int64_t)w_im*tmpz[n34+i].re;
                tmp4 = (int32_t)((accu + 0x40000000) >> 31);

                tmp5 = tmp1 + tmp3;
                tmp1 = tmp1 - tmp3;
                tmp6 = tmp2 + tmp4;
                tmp2 = tmp2 - tmp4;

                tmpz[ n2+i].re = tmpz[   i].re - tmp5;
                tmpz[    i].re = tmpz[   i].re + tmp5;
                tmpz[ n2+i].im = tmpz[   i].im - tmp6;
                tmpz[    i].im = tmpz[   i].im + tmp6;
                tmpz[n34+i].re = tmpz[n4+i].re - tmp2;
                tmpz[ n4+i].re = tmpz[n4+i].re + tmp2;
                tmpz[n34+i].im = tmpz[n4+i].im + tmp1;
                tmpz[ n4+i].im = tmpz[n4+i].im - tmp1;

                w_re_ptr += step;
                w_im_ptr -= step;
            }
        }
        step >>= 1;
        n4   <<= 1;
    }
}
