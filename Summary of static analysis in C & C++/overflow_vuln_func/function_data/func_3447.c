static inline void RENAME(yuv2yuvX)(int16_t *lumFilter, int16_t **lumSrc, int lumFilterSize,
				    int16_t *chrFilter, int16_t **chrSrc, int chrFilterSize,
				    uint8_t *dest, uint8_t *uDest, uint8_t *vDest, int dstW,
				    int16_t * lumMmxFilter, int16_t * chrMmxFilter)
{
#ifdef HAVE_MMX
	if(uDest != NULL)
	{
		asm volatile(
				YSCALEYUV2YV12X(0)
				:: "m" (-chrFilterSize), "r" (chrSrc+chrFilterSize),
				"r" (chrMmxFilter+chrFilterSize*4), "r" (uDest), "m" (dstW>>1)
				: "%eax", "%edx", "%esi"
			);

		asm volatile(
				YSCALEYUV2YV12X(4096)
				:: "m" (-chrFilterSize), "r" (chrSrc+chrFilterSize),
				"r" (chrMmxFilter+chrFilterSize*4), "r" (vDest), "m" (dstW>>1)
				: "%eax", "%edx", "%esi"
			);
	}

	asm volatile(
			YSCALEYUV2YV12X(0)
			:: "m" (-lumFilterSize), "r" (lumSrc+lumFilterSize),
			   "r" (lumMmxFilter+lumFilterSize*4), "r" (dest), "m" (dstW)
			: "%eax", "%edx", "%esi"
		);
#else
	//FIXME Optimize (just quickly writen not opti..)
	int i;
	for(i=0; i<dstW; i++)
	{
		int val=0;
		int j;
		for(j=0; j<lumFilterSize; j++)
			val += lumSrc[j][i] * lumFilter[j];

		dest[i]= MIN(MAX(val>>19, 0), 255);
	}

	if(uDest != NULL)
		for(i=0; i<(dstW>>1); i++)
		{
			int u=0;
			int v=0;
			int j;
			for(j=0; j<lumFilterSize; j++)
			{
				u += chrSrc[j][i] * chrFilter[j];
				v += chrSrc[j][i + 2048] * chrFilter[j];
			}

			uDest[i]= MIN(MAX(u>>19, 0), 255);
			vDest[i]= MIN(MAX(v>>19, 0), 255);
		}
#endif
}
