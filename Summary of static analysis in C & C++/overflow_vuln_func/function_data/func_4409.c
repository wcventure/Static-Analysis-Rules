void updateMMXDitherTables(SwsContext *c, int dstY, int lumBufIndex, int chrBufIndex,
                           int lastInLumBuf, int lastInChrBuf)
{
    const int dstH= c->dstH;
    const int flags= c->flags;
    int16_t **lumPixBuf= c->lumPixBuf;
    int16_t **chrUPixBuf= c->chrUPixBuf;
    int16_t **alpPixBuf= c->alpPixBuf;
    const int vLumBufSize= c->vLumBufSize;
    const int vChrBufSize= c->vChrBufSize;
    int32_t *vLumFilterPos= c->vLumFilterPos;
    int32_t *vChrFilterPos= c->vChrFilterPos;
    int16_t *vLumFilter= c->vLumFilter;
    int16_t *vChrFilter= c->vChrFilter;
    int32_t *lumMmxFilter= c->lumMmxFilter;
    int32_t *chrMmxFilter= c->chrMmxFilter;
    int32_t av_unused *alpMmxFilter= c->alpMmxFilter;
    const int vLumFilterSize= c->vLumFilterSize;
    const int vChrFilterSize= c->vChrFilterSize;
    const int chrDstY= dstY>>c->chrDstVSubSample;
    const int firstLumSrcY= vLumFilterPos[dstY]; //First line needed as input
    const int firstChrSrcY= vChrFilterPos[chrDstY]; //First line needed as input

    c->blueDither= ff_dither8[dstY&1];
    if (c->dstFormat == PIX_FMT_RGB555 || c->dstFormat == PIX_FMT_BGR555)
        c->greenDither= ff_dither8[dstY&1];
    else
        c->greenDither= ff_dither4[dstY&1];
    c->redDither= ff_dither8[(dstY+1)&1];
    if (dstY < dstH - 2) {
        const int16_t **lumSrcPtr= (const int16_t **)(void*) lumPixBuf + lumBufIndex + firstLumSrcY - lastInLumBuf + vLumBufSize;
        const int16_t **chrUSrcPtr= (const int16_t **)(void*) chrUPixBuf + chrBufIndex + firstChrSrcY - lastInChrBuf + vChrBufSize;
        const int16_t **alpSrcPtr= (CONFIG_SWSCALE_ALPHA && alpPixBuf) ? (const int16_t **)(void*) alpPixBuf + lumBufIndex + firstLumSrcY - lastInLumBuf + vLumBufSize : NULL;
        int i;

        if (firstLumSrcY < 0 || firstLumSrcY + vLumFilterSize > c->srcH) {
            const int16_t **tmpY = (const int16_t **) lumPixBuf + 2 * vLumBufSize;
            int neg = -firstLumSrcY, i, end = FFMIN(c->srcH - firstLumSrcY, vLumFilterSize);
            for (i = 0; i < neg;            i++)
                tmpY[i] = lumSrcPtr[neg];
            for (     ; i < end;            i++)
                tmpY[i] = lumSrcPtr[i];
            for (     ; i < vLumFilterSize; i++)
                tmpY[i] = tmpY[i-1];
            lumSrcPtr = tmpY;

            if (alpSrcPtr) {
                const int16_t **tmpA = (const int16_t **) alpPixBuf + 2 * vLumBufSize;
                for (i = 0; i < neg;            i++)
                    tmpA[i] = alpSrcPtr[neg];
                for (     ; i < end;            i++)
                    tmpA[i] = alpSrcPtr[i];
                for (     ; i < vLumFilterSize; i++)
                    tmpA[i] = tmpA[i - 1];
                alpSrcPtr = tmpA;
            }
        }
        if (firstChrSrcY < 0 || firstChrSrcY + vChrFilterSize > c->chrSrcH) {
            const int16_t **tmpU = (const int16_t **) chrUPixBuf + 2 * vChrBufSize;
            int neg = -firstChrSrcY, i, end = FFMIN(c->chrSrcH - firstChrSrcY, vChrFilterSize);
            for (i = 0; i < neg;            i++) {
                tmpU[i] = chrUSrcPtr[neg];
            }
            for (     ; i < end;            i++) {
                tmpU[i] = chrUSrcPtr[i];
            }
            for (     ; i < vChrFilterSize; i++) {
                tmpU[i] = tmpU[i - 1];
            }
            chrUSrcPtr = tmpU;
        }

        if (flags & SWS_ACCURATE_RND) {
            int s= APCK_SIZE / 8;
            for (i=0; i<vLumFilterSize; i+=2) {
                *(const void**)&lumMmxFilter[s*i              ]= lumSrcPtr[i  ];
                *(const void**)&lumMmxFilter[s*i+APCK_PTR2/4  ]= lumSrcPtr[i+(vLumFilterSize>1)];
                lumMmxFilter[s*i+APCK_COEF/4  ]=
                lumMmxFilter[s*i+APCK_COEF/4+1]= vLumFilter[dstY*vLumFilterSize + i    ]
                + (vLumFilterSize>1 ? vLumFilter[dstY*vLumFilterSize + i + 1]<<16 : 0);
                if (CONFIG_SWSCALE_ALPHA && alpPixBuf) {
                    *(const void**)&alpMmxFilter[s*i              ]= alpSrcPtr[i  ];
                    *(const void**)&alpMmxFilter[s*i+APCK_PTR2/4  ]= alpSrcPtr[i+(vLumFilterSize>1)];
                    alpMmxFilter[s*i+APCK_COEF/4  ]=
                    alpMmxFilter[s*i+APCK_COEF/4+1]= lumMmxFilter[s*i+APCK_COEF/4  ];
                }
            }
            for (i=0; i<vChrFilterSize; i+=2) {
                *(const void**)&chrMmxFilter[s*i              ]= chrUSrcPtr[i  ];
                *(const void**)&chrMmxFilter[s*i+APCK_PTR2/4  ]= chrUSrcPtr[i+(vChrFilterSize>1)];
                chrMmxFilter[s*i+APCK_COEF/4  ]=
                chrMmxFilter[s*i+APCK_COEF/4+1]= vChrFilter[chrDstY*vChrFilterSize + i    ]
                + (vChrFilterSize>1 ? vChrFilter[chrDstY*vChrFilterSize + i + 1]<<16 : 0);
            }
        } else {
            for (i=0; i<vLumFilterSize; i++) {
                *(const void**)&lumMmxFilter[4*i+0]= lumSrcPtr[i];
                lumMmxFilter[4*i+2]=
                lumMmxFilter[4*i+3]=
                ((uint16_t)vLumFilter[dstY*vLumFilterSize + i])*0x10001;
                if (CONFIG_SWSCALE_ALPHA && alpPixBuf) {
                    *(const void**)&alpMmxFilter[4*i+0]= alpSrcPtr[i];
                    alpMmxFilter[4*i+2]=
                    alpMmxFilter[4*i+3]= lumMmxFilter[4*i+2];
                }
            }
            for (i=0; i<vChrFilterSize; i++) {
                *(const void**)&chrMmxFilter[4*i+0]= chrUSrcPtr[i];
                chrMmxFilter[4*i+2]=
                chrMmxFilter[4*i+3]=
                ((uint16_t)vChrFilter[chrDstY*vChrFilterSize + i])*0x10001;
            }
        }
    }
}
