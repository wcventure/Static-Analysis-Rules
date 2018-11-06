static av_cold int initFilter(int16_t **outFilter, int32_t **filterPos,
                              int *outFilterSize, int xInc, int srcW,
                              int dstW, int filterAlign, int one,
                              int flags, int cpu_flags,
                              SwsVector *srcFilter, SwsVector *dstFilter,
                              double param[2], int srcPos, int dstPos)
{
    int i;
    int filterSize;
    int filter2Size;
    int minFilterSize;
    int64_t *filter    = NULL;
    int64_t *filter2   = NULL;
    const int64_t fone = 1LL << (54 - FFMIN(av_log2(srcW/dstW), 8));
    int ret            = -1;

    emms_c(); // FIXME should not be required but IS (even for non-MMX versions)

    // NOTE: the +3 is for the MMX(+1) / SSE(+3) scaler which reads over the end
    FF_ALLOC_OR_GOTO(NULL, *filterPos, (dstW + 3) * sizeof(**filterPos), fail);

    if (FFABS(xInc - 0x10000) < 10 && srcPos == dstPos) { // unscaled
        int i;
        filterSize = 1;
        FF_ALLOCZ_OR_GOTO(NULL, filter,
                          dstW * sizeof(*filter) * filterSize, fail);

        for (i = 0; i < dstW; i++) {
            filter[i * filterSize] = fone;
            (*filterPos)[i]        = i;
        }
    } else if (flags & SWS_POINT) { // lame looking point sampling mode
        int i;
        int64_t xDstInSrc;
        filterSize = 1;
        FF_ALLOC_OR_GOTO(NULL, filter,
                         dstW * sizeof(*filter) * filterSize, fail);

        xDstInSrc = ((dstPos*(int64_t)xInc)>>8) - ((srcPos*0x8000)>>7);
        for (i = 0; i < dstW; i++) {
            int xx = (xDstInSrc - ((filterSize - 1) << 15) + (1 << 15)) >> 16;

            (*filterPos)[i] = xx;
            filter[i]       = fone;
            xDstInSrc      += xInc;
        }
    } else if ((xInc <= (1 << 16) && (flags & SWS_AREA)) ||
               (flags & SWS_FAST_BILINEAR)) { // bilinear upscale
        int i;
        int64_t xDstInSrc;
        filterSize = 2;
        FF_ALLOC_OR_GOTO(NULL, filter,
                         dstW * sizeof(*filter) * filterSize, fail);

        xDstInSrc = ((dstPos*(int64_t)xInc)>>8) - ((srcPos*0x8000)>>7);
        for (i = 0; i < dstW; i++) {
            int xx = (xDstInSrc - ((filterSize - 1) << 15) + (1 << 15)) >> 16;
            int j;

            (*filterPos)[i] = xx;
            // bilinear upscale / linear interpolate / area averaging
            for (j = 0; j < filterSize; j++) {
                int64_t coeff= fone - FFABS(((int64_t)xx<<16) - xDstInSrc)*(fone>>16);
                if (coeff < 0)
                    coeff = 0;
                filter[i * filterSize + j] = coeff;
                xx++;
            }
            xDstInSrc += xInc;
        }
    } else {
        int64_t xDstInSrc;
        int sizeFactor;

        if (flags & SWS_BICUBIC)
            sizeFactor = 4;
        else if (flags & SWS_X)
            sizeFactor = 8;
        else if (flags & SWS_AREA)
            sizeFactor = 1;     // downscale only, for upscale it is bilinear
        else if (flags & SWS_GAUSS)
            sizeFactor = 8;     // infinite ;)
        else if (flags & SWS_LANCZOS)
            sizeFactor = param[0] != SWS_PARAM_DEFAULT ? ceil(2 * param[0]) : 6;
        else if (flags & SWS_SINC)
            sizeFactor = 20;    // infinite ;)
        else if (flags & SWS_SPLINE)
            sizeFactor = 20;    // infinite ;)
        else if (flags & SWS_BILINEAR)
            sizeFactor = 2;
        else {
            av_assert0(0);
        }

        if (xInc <= 1 << 16)
            filterSize = 1 + sizeFactor;    // upscale
        else
            filterSize = 1 + (sizeFactor * srcW + dstW - 1) / dstW;

        filterSize = FFMIN(filterSize, srcW - 2);
        filterSize = FFMAX(filterSize, 1);

        FF_ALLOC_OR_GOTO(NULL, filter,
                         dstW * sizeof(*filter) * filterSize, fail);

        xDstInSrc = ((dstPos*(int64_t)xInc)>>7) - ((srcPos*0x10000)>>7);
        for (i = 0; i < dstW; i++) {
            int xx = (xDstInSrc - ((filterSize - 2) << 16)) / (1 << 17);
            int j;
            (*filterPos)[i] = xx;
            for (j = 0; j < filterSize; j++) {
                int64_t d = (FFABS(((int64_t)xx << 17) - xDstInSrc)) << 13;
                double floatd;
                int64_t coeff;

                if (xInc > 1 << 16)
                    d = d * dstW / srcW;
                floatd = d * (1.0 / (1 << 30));

                if (flags & SWS_BICUBIC) {
                    int64_t B = (param[0] != SWS_PARAM_DEFAULT ? param[0] :   0) * (1 << 24);
                    int64_t C = (param[1] != SWS_PARAM_DEFAULT ? param[1] : 0.6) * (1 << 24);

                    if (d >= 1LL << 31) {
                        coeff = 0.0;
                    } else {
                        int64_t dd  = (d  * d) >> 30;
                        int64_t ddd = (dd * d) >> 30;

                        if (d < 1LL << 30)
                            coeff =  (12 * (1 << 24) -  9 * B - 6 * C) * ddd +
                                    (-18 * (1 << 24) + 12 * B + 6 * C) *  dd +
                                      (6 * (1 << 24) -  2 * B)         * (1 << 30);
                        else
                            coeff =      (-B -  6 * C) * ddd +
                                      (6 * B + 30 * C) * dd  +
                                    (-12 * B - 48 * C) * d   +
                                      (8 * B + 24 * C) * (1 << 30);
                    }
                    coeff /= (1LL<<54)/fone;
                }
#if 0
                else if (flags & SWS_X) {
                    double p  = param ? param * 0.01 : 0.3;
                    coeff     = d ? sin(d * M_PI) / (d * M_PI) : 1.0;
                    coeff    *= pow(2.0, -p * d * d);
                }
#endif
                else if (flags & SWS_X) {
                    double A = param[0] != SWS_PARAM_DEFAULT ? param[0] : 1.0;
                    double c;

                    if (floatd < 1.0)
                        c = cos(floatd * M_PI);
                    else
                        c = -1.0;
                    if (c < 0.0)
                        c = -pow(-c, A);
                    else
                        c = pow(c, A);
                    coeff = (c * 0.5 + 0.5) * fone;
                } else if (flags & SWS_AREA) {
                    int64_t d2 = d - (1 << 29);
                    if (d2 * xInc < -(1LL << (29 + 16)))
                        coeff = 1.0 * (1LL << (30 + 16));
                    else if (d2 * xInc < (1LL << (29 + 16)))
                        coeff = -d2 * xInc + (1LL << (29 + 16));
                    else
                        coeff = 0.0;
                    coeff *= fone >> (30 + 16);
                } else if (flags & SWS_GAUSS) {
                    double p = param[0] != SWS_PARAM_DEFAULT ? param[0] : 3.0;
                    coeff = (pow(2.0, -p * floatd * floatd)) * fone;
                } else if (flags & SWS_SINC) {
                    coeff = (d ? sin(floatd * M_PI) / (floatd * M_PI) : 1.0) * fone;
                } else if (flags & SWS_LANCZOS) {
                    double p = param[0] != SWS_PARAM_DEFAULT ? param[0] : 3.0;
                    coeff = (d ? sin(floatd * M_PI) * sin(floatd * M_PI / p) /
                             (floatd * floatd * M_PI * M_PI / p) : 1.0) * fone;
                    if (floatd > p)
                        coeff = 0;
                } else if (flags & SWS_BILINEAR) {
                    coeff = (1 << 30) - d;
                    if (coeff < 0)
                        coeff = 0;
                    coeff *= fone >> 30;
                } else if (flags & SWS_SPLINE) {
                    double p = -2.196152422706632;
                    coeff = getSplineCoeff(1.0, 0.0, p, -p - 1.0, floatd) * fone;
                } else {
                    av_assert0(0);
                }

                filter[i * filterSize + j] = coeff;
                xx++;
            }
            xDstInSrc += 2 * xInc;
        }
    }

    /
    av_assert0(filterSize > 0);
    filter2Size = filterSize;
    if (srcFilter)
        filter2Size += srcFilter->length - 1;
    if (dstFilter)
        filter2Size += dstFilter->length - 1;
    av_assert0(filter2Size > 0);
    FF_ALLOCZ_OR_GOTO(NULL, filter2, filter2Size * dstW * sizeof(*filter2), fail);

    for (i = 0; i < dstW; i++) {
        int j, k;

        if (srcFilter) {
            for (k = 0; k < srcFilter->length; k++) {
                for (j = 0; j < filterSize; j++)
                    filter2[i * filter2Size + k + j] +=
                        srcFilter->coeff[k] * filter[i * filterSize + j];
            }
        } else {
            for (j = 0; j < filterSize; j++)
                filter2[i * filter2Size + j] = filter[i * filterSize + j];
        }
        // FIXME dstFilter

        (*filterPos)[i] += (filterSize - 1) / 2 - (filter2Size - 1) / 2;
    }
    av_freep(&filter);

    /
    // Assume it is near normalized (*0.5 or *2.0 is OK but * 0.001 is not).
    minFilterSize = 0;
    for (i = dstW - 1; i >= 0; i--) {
        int min = filter2Size;
        int j;
        int64_t cutOff = 0.0;

        /
        for (j = 0; j < filter2Size; j++) {
            int k;
            cutOff += FFABS(filter2[i * filter2Size]);

            if (cutOff > SWS_MAX_REDUCE_CUTOFF * fone)
                break;

            /
            if (i < dstW - 1 && (*filterPos)[i] >= (*filterPos)[i + 1])
                break;

            // move filter coefficients left
            for (k = 1; k < filter2Size; k++)
                filter2[i * filter2Size + k - 1] = filter2[i * filter2Size + k];
            filter2[i * filter2Size + k - 1] = 0;
            (*filterPos)[i]++;
        }

        cutOff = 0;
        /
        for (j = filter2Size - 1; j > 0; j--) {
            cutOff += FFABS(filter2[i * filter2Size + j]);

            if (cutOff > SWS_MAX_REDUCE_CUTOFF * fone)
                break;
            min--;
        }

        if (min > minFilterSize)
            minFilterSize = min;
    }

    if (HAVE_ALTIVEC && cpu_flags & AV_CPU_FLAG_ALTIVEC) {
        // we can handle the special case 4, so we don't want to go the full 8
        if (minFilterSize < 5)
            filterAlign = 4;

        /
        if (minFilterSize < 3)
            filterAlign = 1;
    }

    if (HAVE_MMX && cpu_flags & AV_CPU_FLAG_MMX) {
        // special case for unscaled vertical filtering
        if (minFilterSize == 1 && filterAlign == 2)
            filterAlign = 1;
    }

    av_assert0(minFilterSize > 0);
    filterSize = (minFilterSize + (filterAlign - 1)) & (~(filterAlign - 1));
    av_assert0(filterSize > 0);
    filter = av_malloc(filterSize * dstW * sizeof(*filter));
    if (filterSize >= MAX_FILTER_SIZE * 16 /
                      ((flags & SWS_ACCURATE_RND) ? APCK_SIZE : 16) || !filter) {
        av_log(NULL, AV_LOG_ERROR, "sws: filterSize %d is too large, try less extreem scaling or increase MAX_FILTER_SIZE and recompile\n", filterSize);
        goto fail;
    }
    *outFilterSize = filterSize;

    if (flags & SWS_PRINT_INFO)
        av_log(NULL, AV_LOG_VERBOSE,
               "SwScaler: reducing / aligning filtersize %d -> %d\n",
               filter2Size, filterSize);
    /
    for (i = 0; i < dstW; i++) {
        int j;

        for (j = 0; j < filterSize; j++) {
            if (j >= filter2Size)
                filter[i * filterSize + j] = 0;
            else
                filter[i * filterSize + j] = filter2[i * filter2Size + j];
            if ((flags & SWS_BITEXACT) && j >= minFilterSize)
                filter[i * filterSize + j] = 0;
        }
    }

    // FIXME try to align filterPos if possible

    // fix borders
    for (i = 0; i < dstW; i++) {
        int j;
        if ((*filterPos)[i] < 0) {
            // move filter coefficients left to compensate for filterPos
            for (j = 1; j < filterSize; j++) {
                int left = FFMAX(j + (*filterPos)[i], 0);
                filter[i * filterSize + left] += filter[i * filterSize + j];
                filter[i * filterSize + j]     = 0;
            }
            (*filterPos)[i]= 0;
        }

        if ((*filterPos)[i] + filterSize > srcW) {
            int shift = (*filterPos)[i] + filterSize - srcW;
            // move filter coefficients right to compensate for filterPos
            for (j = filterSize - 2; j >= 0; j--) {
                int right = FFMIN(j + shift, filterSize - 1);
                filter[i * filterSize + right] += filter[i * filterSize + j];
                filter[i * filterSize + j]      = 0;
            }
            (*filterPos)[i]= srcW - filterSize;
        }
    }

    // Note the +1 is for the MMX scaler which reads over the end
    /
    FF_ALLOCZ_OR_GOTO(NULL, *outFilter,
                      *outFilterSize * (dstW + 3) * sizeof(int16_t), fail);

    /
    for (i = 0; i < dstW; i++) {
        int j;
        int64_t error = 0;
        int64_t sum   = 0;

        for (j = 0; j < filterSize; j++) {
            sum += filter[i * filterSize + j];
        }
        sum = (sum + one / 2) / one;
        for (j = 0; j < *outFilterSize; j++) {
            int64_t v = filter[i * filterSize + j] + error;
            int intV  = ROUNDED_DIV(v, sum);
            (*outFilter)[i * (*outFilterSize) + j] = intV;
            error                                  = v - intV * sum;
        }
    }

    (*filterPos)[dstW + 0] =
    (*filterPos)[dstW + 1] =
    (*filterPos)[dstW + 2] = (*filterPos)[dstW - 1]; /
    for (i = 0; i < *outFilterSize; i++) {
        int k = (dstW - 1) * (*outFilterSize) + i;
        (*outFilter)[k + 1 * (*outFilterSize)] =
        (*outFilter)[k + 2 * (*outFilterSize)] =
        (*outFilter)[k + 3 * (*outFilterSize)] = (*outFilter)[k];
    }

    ret = 0;

fail:
    if(ret < 0)
        av_log(NULL, AV_LOG_ERROR, "sws: initFilter failed\n");
    av_free(filter);
    av_free(filter2);
    return ret;
}
