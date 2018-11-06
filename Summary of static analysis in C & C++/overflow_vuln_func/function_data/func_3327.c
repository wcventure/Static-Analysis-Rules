static int planarCopyWrapper(SwsContext *c, const uint8_t* src[], int srcStride[], int srcSliceY,
                             int srcSliceH, uint8_t* dst[], int dstStride[])
{
    int plane, i, j;
    for (plane=0; plane<4; plane++) {
        int length= (plane==0 || plane==3) ? c->srcW  : -((-c->srcW  )>>c->chrDstHSubSample);
        int y=      (plane==0 || plane==3) ? srcSliceY: -((-srcSliceY)>>c->chrDstVSubSample);
        int height= (plane==0 || plane==3) ? srcSliceH: -((-srcSliceH)>>c->chrDstVSubSample);
        const uint8_t *srcPtr= src[plane];
        uint8_t *dstPtr= dst[plane] + dstStride[plane]*y;

        if (!dst[plane]) continue;
        // ignore palette for GRAY8
        if (plane == 1 && !dst[2]) continue;
        if (!src[plane] || (plane == 1 && !src[2])) {
            if(is16BPS(c->dstFormat))
                length*=2;
            fillPlane(dst[plane], dstStride[plane], length, height, y, (plane==3) ? 255 : 128);
        } else {
            if(is9_OR_10BPS(c->srcFormat)) {
                const int src_depth = av_pix_fmt_descriptors[c->srcFormat].comp[plane].depth_minus1+1;
                const int dst_depth = av_pix_fmt_descriptors[c->dstFormat].comp[plane].depth_minus1+1;
                const uint16_t *srcPtr2 = (const uint16_t*)srcPtr;

                if (is16BPS(c->dstFormat)) {
                    uint16_t *dstPtr2 = (uint16_t*)dstPtr;
#define COPY9_OR_10TO16(rfunc, wfunc) \
                    for (i = 0; i < height; i++) { \
                        for (j = 0; j < length; j++) { \
                            int srcpx = rfunc(&srcPtr2[j]); \
                            wfunc(&dstPtr2[j], (srcpx<<(16-src_depth)) | (srcpx>>(2*src_depth-16))); \
                        } \
                        dstPtr2 += dstStride[plane]/2; \
                        srcPtr2 += srcStride[plane]/2; \
                    }
                    if (isBE(c->dstFormat)) {
                        if (isBE(c->srcFormat)) {
                            COPY9_OR_10TO16(AV_RB16, AV_WB16);
                        } else {
                            COPY9_OR_10TO16(AV_RL16, AV_WB16);
                        }
                    } else {
                        if (isBE(c->srcFormat)) {
                            COPY9_OR_10TO16(AV_RB16, AV_WL16);
                        } else {
                            COPY9_OR_10TO16(AV_RL16, AV_WL16);
                        }
                    }
                } else if (is9_OR_10BPS(c->dstFormat)) {
                    uint16_t *dstPtr2 = (uint16_t*)dstPtr;
#define COPY9_OR_10TO9_OR_10(loop) \
                    for (i = 0; i < height; i++) { \
                        for (j = 0; j < length; j++) { \
                            loop; \
                        } \
                        dstPtr2 += dstStride[plane]/2; \
                        srcPtr2 += srcStride[plane]/2; \
                    }
#define COPY9_OR_10TO9_OR_10_2(rfunc, wfunc) \
                    if (dst_depth > src_depth) { \
                        COPY9_OR_10TO9_OR_10(int srcpx = rfunc(&srcPtr2[j]); \
                            wfunc(&dstPtr2[j], (srcpx << 1) | (srcpx >> 9))); \
                    } else if (dst_depth < src_depth) { \
                        DITHER_COPY(dstPtr2, dstStride[plane]/2, wfunc, \
                                    srcPtr2, srcStride[plane]/2, rfunc, \
                                    dither_8x8_1, 1); \
                    } else { \
                        COPY9_OR_10TO9_OR_10(wfunc(&dstPtr2[j], rfunc(&srcPtr2[j]))); \
                    }
                    if (isBE(c->dstFormat)) {
                        if (isBE(c->srcFormat)) {
                            COPY9_OR_10TO9_OR_10_2(AV_RB16, AV_WB16);
                        } else {
                            COPY9_OR_10TO9_OR_10_2(AV_RL16, AV_WB16);
                        }
                    } else {
                        if (isBE(c->srcFormat)) {
                            COPY9_OR_10TO9_OR_10_2(AV_RB16, AV_WL16);
                        } else {
                            COPY9_OR_10TO9_OR_10_2(AV_RL16, AV_WL16);
                        }
                    }
                } else {
#define W8(a, b) { *(a) = (b); }
#define COPY9_OR_10TO8(rfunc) \
                    if (src_depth == 9) { \
                        DITHER_COPY(dstPtr,  dstStride[plane],   W8, \
                                    srcPtr2, srcStride[plane]/2, rfunc, \
                                    dither_8x8_1, 1); \
                    } else { \
                        DITHER_COPY(dstPtr,  dstStride[plane],   W8, \
                                    srcPtr2, srcStride[plane]/2, rfunc, \
                                    dither_8x8_3, 2); \
                    }
                    if (isBE(c->srcFormat)) {
                        COPY9_OR_10TO8(AV_RB16);
                    } else {
                        COPY9_OR_10TO8(AV_RL16);
                    }
                }
            } else if(is9_OR_10BPS(c->dstFormat)) {
                const int dst_depth = av_pix_fmt_descriptors[c->dstFormat].comp[plane].depth_minus1+1;
                uint16_t *dstPtr2 = (uint16_t*)dstPtr;

                if (is16BPS(c->srcFormat)) {
                    const uint16_t *srcPtr2 = (const uint16_t*)srcPtr;
#define COPY16TO9_OR_10(rfunc, wfunc) \
                    if (dst_depth == 9) { \
                        DITHER_COPY(dstPtr2, dstStride[plane]/2, wfunc, \
                                    srcPtr2, srcStride[plane]/2, rfunc, \
                                    dither_8x8_128, 7); \
                    } else { \
                        DITHER_COPY(dstPtr2, dstStride[plane]/2, wfunc, \
                                    srcPtr2, srcStride[plane]/2, rfunc, \
                                    dither_8x8_64, 6); \
                    }
                    if (isBE(c->dstFormat)) {
                        if (isBE(c->srcFormat)) {
                            COPY16TO9_OR_10(AV_RB16, AV_WB16);
                        } else {
                            COPY16TO9_OR_10(AV_RL16, AV_WB16);
                        }
                    } else {
                        if (isBE(c->srcFormat)) {
                            COPY16TO9_OR_10(AV_RB16, AV_WL16);
                        } else {
                            COPY16TO9_OR_10(AV_RL16, AV_WL16);
                        }
                    }
                } else / {
#define COPY8TO9_OR_10(wfunc) \
                    for (i = 0; i < height; i++) { \
                        for (j = 0; j < length; j++) { \
                            const int srcpx = srcPtr[j]; \
                            wfunc(&dstPtr2[j], (srcpx<<(dst_depth-8)) | (srcpx >> (16-dst_depth))); \
                        } \
                        dstPtr2 += dstStride[plane]/2; \
                        srcPtr  += srcStride[plane]; \
                    }
                    if (isBE(c->dstFormat)) {
                        COPY8TO9_OR_10(AV_WB16);
                    } else {
                        COPY8TO9_OR_10(AV_WL16);
                    }
                }
            } else if(is16BPS(c->srcFormat) && !is16BPS(c->dstFormat)) {
                const uint16_t *srcPtr2 = (const uint16_t*)srcPtr;
#define COPY16TO8(rfunc) \
                    DITHER_COPY(dstPtr,  dstStride[plane],   W8, \
                                srcPtr2, srcStride[plane]/2, rfunc, \
                                dither_8x8_256, 8);
                if (isBE(c->srcFormat)) {
                    COPY16TO8(AV_RB16);
                } else {
                    COPY16TO8(AV_RL16);
                }
            } else if(!is16BPS(c->srcFormat) && is16BPS(c->dstFormat)) {
                for (i=0; i<height; i++) {
                    for (j=0; j<length; j++) {
                        dstPtr[ j<<1   ] = srcPtr[j];
                        dstPtr[(j<<1)+1] = srcPtr[j];
                    }
                    srcPtr+= srcStride[plane];
                    dstPtr+= dstStride[plane];
                }
            } else if(is16BPS(c->srcFormat) && is16BPS(c->dstFormat)
                  && isBE(c->srcFormat) != isBE(c->dstFormat)) {

                for (i=0; i<height; i++) {
                    for (j=0; j<length; j++)
                        ((uint16_t*)dstPtr)[j] = av_bswap16(((const uint16_t*)srcPtr)[j]);
                    srcPtr+= srcStride[plane];
                    dstPtr+= dstStride[plane];
                }
            } else if (dstStride[plane] == srcStride[plane] &&
                       srcStride[plane] > 0 && srcStride[plane] == length) {
                memcpy(dst[plane] + dstStride[plane]*y, src[plane],
                       height*dstStride[plane]);
            } else {
                if(is16BPS(c->srcFormat) && is16BPS(c->dstFormat))
                    length*=2;
                for (i=0; i<height; i++) {
                    memcpy(dstPtr, srcPtr, length);
                    srcPtr+= srcStride[plane];
                    dstPtr+= dstStride[plane];
                }
            }
        }
    }
    return srcSliceH;
}
