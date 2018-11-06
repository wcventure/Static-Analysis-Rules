// test by ref -> src -> dst -> out & compare out against ref
// ref & out are YV12
static int doTest(uint8_t *ref[4], int refStride[4], int w, int h,
                  enum PixelFormat srcFormat, enum PixelFormat dstFormat,
                  int srcW, int srcH, int dstW, int dstH, int flags)
{
    uint8_t *src[4] = {0};
    uint8_t *dst[4] = {0};
    uint8_t *out[4] = {0};
    int srcStride[4], dstStride[4];
    int i;
    uint64_t ssdY, ssdU=0, ssdV=0, ssdA=0;
    struct SwsContext *srcContext = NULL, *dstContext = NULL,
                      *outContext = NULL;
    int res;

    res = 0;
    for (i=0; i<4; i++) {
        // avoid stride % bpp != 0
        if (srcFormat==PIX_FMT_RGB24 || srcFormat==PIX_FMT_BGR24)
            srcStride[i]= srcW*3;
        else if (srcFormat==PIX_FMT_RGB48BE || srcFormat==PIX_FMT_RGB48LE)
            srcStride[i]= srcW*6;
        else
            srcStride[i]= srcW*4;

        if (dstFormat==PIX_FMT_RGB24 || dstFormat==PIX_FMT_BGR24)
            dstStride[i]= dstW*3;
        else if (dstFormat==PIX_FMT_RGB48BE || dstFormat==PIX_FMT_RGB48LE)
            dstStride[i]= dstW*6;
        else
            dstStride[i]= dstW*4;

        /
        src[i]= av_mallocz(srcStride[i]*srcH);
        dst[i]= av_mallocz(dstStride[i]*dstH);
        out[i]= av_mallocz(refStride[i]*h);
        if (!src[i] || !dst[i] || !out[i]) {
            perror("Malloc");
            res = -1;

            goto end;
        }
    }

    srcContext= sws_getContext(w, h, PIX_FMT_YUVA420P, srcW, srcH, srcFormat, flags, NULL, NULL, NULL);
    if (!srcContext) {
        fprintf(stderr, "Failed to get %s ---> %s\n",
                av_pix_fmt_descriptors[PIX_FMT_YUVA420P].name,
                av_pix_fmt_descriptors[srcFormat].name);
        res = -1;

        goto end;
    }
    dstContext= sws_getContext(srcW, srcH, srcFormat, dstW, dstH, dstFormat, flags, NULL, NULL, NULL);
    if (!dstContext) {
        fprintf(stderr, "Failed to get %s ---> %s\n",
                av_pix_fmt_descriptors[srcFormat].name,
                av_pix_fmt_descriptors[dstFormat].name);
        res = -1;

        goto end;
    }
    outContext= sws_getContext(dstW, dstH, dstFormat, w, h, PIX_FMT_YUVA420P, flags, NULL, NULL, NULL);
    if (!outContext) {
        fprintf(stderr, "Failed to get %s ---> %s\n",
                av_pix_fmt_descriptors[dstFormat].name,
                av_pix_fmt_descriptors[PIX_FMT_YUVA420P].name);
        res = -1;

        goto end;
    }
//    printf("test %X %X %X -> %X %X %X\n", (int)ref[0], (int)ref[1], (int)ref[2],
//        (int)src[0], (int)src[1], (int)src[2]);

    sws_scale(srcContext, ref, refStride, 0, h   , src, srcStride);
    sws_scale(dstContext, src, srcStride, 0, srcH, dst, dstStride);
    sws_scale(outContext, dst, dstStride, 0, dstH, out, refStride);

    ssdY= getSSD(ref[0], out[0], refStride[0], refStride[0], w, h);
    if (hasChroma(srcFormat) && hasChroma(dstFormat)) {
        //FIXME check that output is really gray
        ssdU= getSSD(ref[1], out[1], refStride[1], refStride[1], (w+1)>>1, (h+1)>>1);
        ssdV= getSSD(ref[2], out[2], refStride[2], refStride[2], (w+1)>>1, (h+1)>>1);
    }
    if (isALPHA(srcFormat) && isALPHA(dstFormat))
        ssdA= getSSD(ref[3], out[3], refStride[3], refStride[3], w, h);

    ssdY/= w*h;
    ssdU/= w*h/4;
    ssdV/= w*h/4;
    ssdA/= w*h;

    printf(" %s %dx%d -> %s %4dx%4d flags=%2d SSD=%5"PRId64",%5"PRId64",%5"PRId64",%5"PRId64"\n",
           av_pix_fmt_descriptors[srcFormat].name, srcW, srcH,
           av_pix_fmt_descriptors[dstFormat].name, dstW, dstH,
           flags, ssdY, ssdU, ssdV, ssdA);
    fflush(stdout);

end:

    sws_freeContext(srcContext);
    sws_freeContext(dstContext);
    sws_freeContext(outContext);

    for (i=0; i<4; i++) {
        av_free(src[i]);
        av_free(dst[i]);
        av_free(out[i]);
    }

    return res;
}
