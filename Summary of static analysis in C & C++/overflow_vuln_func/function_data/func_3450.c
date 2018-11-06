// Bilinear / Bicubic scaling
static inline void RENAME(hScale)(int16_t *dst, int dstW, uint8_t *src, int srcW, int xInc,
				  int16_t *filter, int16_t *filterPos, int filterSize)
{
#ifdef HAVE_MMX
	if(filterSize==4) // allways true for upscaling, sometimes for down too
	{
		int counter= -2*dstW;
		filter-= counter*2;
		filterPos-= counter/2;
		dst-= counter/2;
		asm volatile(
			"pxor %%mm7, %%mm7		\n\t"
			"movq w02, %%mm6		\n\t"
			"pushl %%ebp			\n\t" // we use 7 regs here ...
			"movl %%eax, %%ebp		\n\t"
			".balign 16			\n\t"
			"1:				\n\t"
			"movzwl (%2, %%ebp), %%eax	\n\t"
			"movzwl 2(%2, %%ebp), %%ebx	\n\t"
			"movq (%1, %%ebp, 4), %%mm1	\n\t"
			"movq 8(%1, %%ebp, 4), %%mm3	\n\t"
			"movd (%3, %%eax), %%mm0	\n\t"
			"movd (%3, %%ebx), %%mm2	\n\t"
			"punpcklbw %%mm7, %%mm0		\n\t"
			"punpcklbw %%mm7, %%mm2		\n\t"
			"pmaddwd %%mm1, %%mm0		\n\t"
			"pmaddwd %%mm2, %%mm3		\n\t"
			"psrad $8, %%mm0		\n\t"
			"psrad $8, %%mm3		\n\t"
			"packssdw %%mm3, %%mm0		\n\t"
			"pmaddwd %%mm6, %%mm0		\n\t"
			"packssdw %%mm0, %%mm0		\n\t"
			"movd %%mm0, (%4, %%ebp)	\n\t"
			"addl $4, %%ebp			\n\t"
			" jnc 1b			\n\t"
			
			"popl %%ebp			\n\t"
			: "+a" (counter)
			: "c" (filter), "d" (filterPos), "S" (src), "D" (dst)
			: "%ebx"
		);
	}
	else if(filterSize==8)
	{
		int counter= -2*dstW;
		filter-= counter*4;
		filterPos-= counter/2;
		dst-= counter/2;
		asm volatile(
			"pxor %%mm7, %%mm7		\n\t"
			"movq w02, %%mm6		\n\t"
			"pushl %%ebp			\n\t" // we use 7 regs here ...
			"movl %%eax, %%ebp		\n\t"
			".balign 16			\n\t"
			"1:				\n\t"
			"movzwl (%2, %%ebp), %%eax	\n\t"
			"movzwl 2(%2, %%ebp), %%ebx	\n\t"
			"movq (%1, %%ebp, 8), %%mm1	\n\t"
			"movq 16(%1, %%ebp, 8), %%mm3	\n\t"
			"movd (%3, %%eax), %%mm0	\n\t"
			"movd (%3, %%ebx), %%mm2	\n\t"
			"punpcklbw %%mm7, %%mm0		\n\t"
			"punpcklbw %%mm7, %%mm2		\n\t"
			"pmaddwd %%mm1, %%mm0		\n\t"
			"pmaddwd %%mm2, %%mm3		\n\t"

			"movq 8(%1, %%ebp, 8), %%mm1	\n\t"
			"movq 24(%1, %%ebp, 8), %%mm5	\n\t"
			"movd 4(%3, %%eax), %%mm4	\n\t"
			"movd 4(%3, %%ebx), %%mm2	\n\t"
			"punpcklbw %%mm7, %%mm4		\n\t"
			"punpcklbw %%mm7, %%mm2		\n\t"
			"pmaddwd %%mm1, %%mm4		\n\t"
			"pmaddwd %%mm2, %%mm5		\n\t"
			"paddd %%mm4, %%mm0		\n\t"
			"paddd %%mm5, %%mm3		\n\t"
						
			"psrad $8, %%mm0		\n\t"
			"psrad $8, %%mm3		\n\t"
			"packssdw %%mm3, %%mm0		\n\t"
			"pmaddwd %%mm6, %%mm0		\n\t"
			"packssdw %%mm0, %%mm0		\n\t"
			"movd %%mm0, (%4, %%ebp)	\n\t"
			"addl $4, %%ebp			\n\t"
			" jnc 1b			\n\t"

			"popl %%ebp			\n\t"
			: "+a" (counter)
			: "c" (filter), "d" (filterPos), "S" (src), "D" (dst)
			: "%ebx"
		);
	}
	else
	{
		int counter= -2*dstW;
//		filter-= counter*filterSize/2;
		filterPos-= counter/2;
		dst-= counter/2;
		asm volatile(
			"pxor %%mm7, %%mm7		\n\t"
			"movq w02, %%mm6		\n\t"
			".balign 16			\n\t"
			"1:				\n\t"
			"movl %2, %%ecx			\n\t"
			"movzwl (%%ecx, %0), %%eax	\n\t"
			"movzwl 2(%%ecx, %0), %%ebx	\n\t"
			"movl %5, %%ecx			\n\t"
			"pxor %%mm4, %%mm4		\n\t"
			"pxor %%mm5, %%mm5		\n\t"
			"2:				\n\t"
			"movq (%1), %%mm1		\n\t"
			"movq (%1, %6), %%mm3		\n\t"
			"movd (%%ecx, %%eax), %%mm0	\n\t"
			"movd (%%ecx, %%ebx), %%mm2	\n\t"
			"punpcklbw %%mm7, %%mm0		\n\t"
			"punpcklbw %%mm7, %%mm2		\n\t"
			"pmaddwd %%mm1, %%mm0		\n\t"
			"pmaddwd %%mm2, %%mm3		\n\t"
			"paddd %%mm3, %%mm5		\n\t"
			"paddd %%mm0, %%mm4		\n\t"
			"addl $8, %1			\n\t"
			"addl $4, %%ecx			\n\t"
			"cmpl %4, %%ecx			\n\t"
			" jb 2b				\n\t"
			"addl %6, %1			\n\t"
			"psrad $8, %%mm4		\n\t"
			"psrad $8, %%mm5		\n\t"
			"packssdw %%mm5, %%mm4		\n\t"
			"pmaddwd %%mm6, %%mm4		\n\t"
			"packssdw %%mm4, %%mm4		\n\t"
			"movl %3, %%eax			\n\t"
			"movd %%mm4, (%%eax, %0)	\n\t"
			"addl $4, %0			\n\t"
			" jnc 1b			\n\t"

			: "+r" (counter)
			: "r" (filter), "m" (filterPos), "m" (dst), "m"(src+filterSize),
			  "m" (src), "r" (filterSize*2)
			: "%ebx", "%eax", "%ecx"
		);
	}
#else
	int i;
	for(i=0; i<dstW; i++)
	{
		int j;
		int srcPos= filterPos[i];
		int val=0;
//		printf("filterPos: %d\n", filterPos[i]);
		for(j=0; j<filterSize; j++)
		{
//			printf("filter: %d, src: %d\n", filter[i], src[srcPos + j]);
			val += ((int)src[srcPos + j])*filter[filterSize*i + j];
		}
//		filter += hFilterSize;
		dst[i] = MIN(MAX(0, val>>7), (1<<15)-1); // the cubic equation does overflow ...
//		dst[i] = val>>7;
	}
#endif
}
