static inline void RENAME(yuv2rgb1)(uint16_t *buf0, uint16_t *uvbuf0, uint16_t *uvbuf1,
			    uint8_t *dest, int dstW, int uvalpha, int dstbpp)
{
	int uvalpha1=uvalpha^4095;
	const int yalpha=0;
	const int yalpha1=0;

	if(fullUVIpol || allwaysIpol)
	{
		RENAME(yuv2rgb2)(buf0, buf0, uvbuf0, uvbuf1, dest, dstW, 0, uvalpha, dstbpp);
		return;
	}

#ifdef HAVE_MMX
	if( uvalpha < 2048 ) // note this is not correct (shifts chrominance by 0.5 pixels) but its a bit faster
	{
		if(dstbpp == 32)
		{
			asm volatile(
				YSCALEYUV2RGB1
				WRITEBGR32
			:: "r" (buf0), "r" (buf0), "r" (uvbuf0), "r" (uvbuf1), "r" (dest), "m" (dstW),
			"m" (yalpha1), "m" (uvalpha1)
			: "%eax"
			);
		}
		else if(dstbpp==24)
		{
			asm volatile(
				"movl %4, %%ebx			\n\t"
				YSCALEYUV2RGB1
				WRITEBGR24
			:: "r" (buf0), "r" (buf0), "r" (uvbuf0), "r" (uvbuf1), "m" (dest), "m" (dstW),
			"m" (yalpha1), "m" (uvalpha1)
			: "%eax", "%ebx"
			);
		}
		else if(dstbpp==15)
		{
			asm volatile(
				YSCALEYUV2RGB1
		/
#ifdef DITHER1XBPP
				"paddusb b5Dither, %%mm2	\n\t"
				"paddusb g5Dither, %%mm4	\n\t"
				"paddusb r5Dither, %%mm5	\n\t"
#endif
				WRITEBGR15
			:: "r" (buf0), "r" (buf0), "r" (uvbuf0), "r" (uvbuf1), "r" (dest), "m" (dstW),
			"m" (yalpha1), "m" (uvalpha1)
			: "%eax"
			);
		}
		else if(dstbpp==16)
		{
			asm volatile(
				YSCALEYUV2RGB1
		/
#ifdef DITHER1XBPP
				"paddusb b5Dither, %%mm2	\n\t"
				"paddusb g6Dither, %%mm4	\n\t"
				"paddusb r5Dither, %%mm5	\n\t"
#endif

				WRITEBGR16
			:: "r" (buf0), "r" (buf0), "r" (uvbuf0), "r" (uvbuf1), "r" (dest), "m" (dstW),
			"m" (yalpha1), "m" (uvalpha1)
			: "%eax"
			);
		}
	}
	else
	{
		if(dstbpp == 32)
		{
			asm volatile(
				YSCALEYUV2RGB1b
				WRITEBGR32
			:: "r" (buf0), "r" (buf0), "r" (uvbuf0), "r" (uvbuf1), "r" (dest), "m" (dstW),
			"m" (yalpha1), "m" (uvalpha1)
			: "%eax"
			);
		}
		else if(dstbpp==24)
		{
			asm volatile(
				"movl %4, %%ebx			\n\t"
				YSCALEYUV2RGB1b
				WRITEBGR24
			:: "r" (buf0), "r" (buf0), "r" (uvbuf0), "r" (uvbuf1), "m" (dest), "m" (dstW),
			"m" (yalpha1), "m" (uvalpha1)
			: "%eax", "%ebx"
			);
		}
		else if(dstbpp==15)
		{
			asm volatile(
				YSCALEYUV2RGB1b
		/
#ifdef DITHER1XBPP
				"paddusb b5Dither, %%mm2	\n\t"
				"paddusb g5Dither, %%mm4	\n\t"
				"paddusb r5Dither, %%mm5	\n\t"
#endif
				WRITEBGR15
			:: "r" (buf0), "r" (buf0), "r" (uvbuf0), "r" (uvbuf1), "r" (dest), "m" (dstW),
			"m" (yalpha1), "m" (uvalpha1)
			: "%eax"
			);
		}
		else if(dstbpp==16)
		{
			asm volatile(
				YSCALEYUV2RGB1b
		/
#ifdef DITHER1XBPP
				"paddusb b5Dither, %%mm2	\n\t"
				"paddusb g6Dither, %%mm4	\n\t"
				"paddusb r5Dither, %%mm5	\n\t"
#endif

				WRITEBGR16
			:: "r" (buf0), "r" (buf0), "r" (uvbuf0), "r" (uvbuf1), "r" (dest), "m" (dstW),
			"m" (yalpha1), "m" (uvalpha1)
			: "%eax"
			);
		}
	}
#else
//FIXME write 2 versions (for even & odd lines)

	if(dstbpp==32)
	{
		int i;
		for(i=0; i<dstW-1; i+=2){
			// vertical linear interpolation && yuv2rgb in a single step:
			int Y1=yuvtab_2568[buf0[i]>>7];
			int Y2=yuvtab_2568[buf0[i+1]>>7];
			int U=((uvbuf0[i>>1]*uvalpha1+uvbuf1[i>>1]*uvalpha)>>19);
			int V=((uvbuf0[(i>>1)+2048]*uvalpha1+uvbuf1[(i>>1)+2048]*uvalpha)>>19);

			int Cb= yuvtab_40cf[U];
			int Cg= yuvtab_1a1e[V] + yuvtab_0c92[U];
			int Cr= yuvtab_3343[V];

			dest[4*i+0]=clip_table[((Y1 + Cb) >>13)];
			dest[4*i+1]=clip_table[((Y1 + Cg) >>13)];
			dest[4*i+2]=clip_table[((Y1 + Cr) >>13)];

			dest[4*i+4]=clip_table[((Y2 + Cb) >>13)];
			dest[4*i+5]=clip_table[((Y2 + Cg) >>13)];
			dest[4*i+6]=clip_table[((Y2 + Cr) >>13)];
		}
	}
	else if(dstbpp==24)
	{
		int i;
		for(i=0; i<dstW-1; i+=2){
			// vertical linear interpolation && yuv2rgb in a single step:
			int Y1=yuvtab_2568[buf0[i]>>7];
			int Y2=yuvtab_2568[buf0[i+1]>>7];
			int U=((uvbuf0[i>>1]*uvalpha1+uvbuf1[i>>1]*uvalpha)>>19);
			int V=((uvbuf0[(i>>1)+2048]*uvalpha1+uvbuf1[(i>>1)+2048]*uvalpha)>>19);

			int Cb= yuvtab_40cf[U];
			int Cg= yuvtab_1a1e[V] + yuvtab_0c92[U];
			int Cr= yuvtab_3343[V];

			dest[0]=clip_table[((Y1 + Cb) >>13)];
			dest[1]=clip_table[((Y1 + Cg) >>13)];
			dest[2]=clip_table[((Y1 + Cr) >>13)];

			dest[3]=clip_table[((Y2 + Cb) >>13)];
			dest[4]=clip_table[((Y2 + Cg) >>13)];
			dest[5]=clip_table[((Y2 + Cr) >>13)];
			dest+=6;
		}
	}
	else if(dstbpp==16)
	{
		int i;
		for(i=0; i<dstW-1; i+=2){
			// vertical linear interpolation && yuv2rgb in a single step:
			int Y1=yuvtab_2568[buf0[i]>>7];
			int Y2=yuvtab_2568[buf0[i+1]>>7];
			int U=((uvbuf0[i>>1]*uvalpha1+uvbuf1[i>>1]*uvalpha)>>19);
			int V=((uvbuf0[(i>>1)+2048]*uvalpha1+uvbuf1[(i>>1)+2048]*uvalpha)>>19);

			int Cb= yuvtab_40cf[U];
			int Cg= yuvtab_1a1e[V] + yuvtab_0c92[U];
			int Cr= yuvtab_3343[V];

			((uint16_t*)dest)[i] =
				clip_table16b[(Y1 + Cb) >>13] |
				clip_table16g[(Y1 + Cg) >>13] |
				clip_table16r[(Y1 + Cr) >>13];

			((uint16_t*)dest)[i+1] =
				clip_table16b[(Y2 + Cb) >>13] |
				clip_table16g[(Y2 + Cg) >>13] |
				clip_table16r[(Y2 + Cr) >>13];
		}
	}
	else if(dstbpp==15)
	{
		int i;
		for(i=0; i<dstW-1; i+=2){
			// vertical linear interpolation && yuv2rgb in a single step:
			int Y1=yuvtab_2568[buf0[i]>>7];
			int Y2=yuvtab_2568[buf0[i+1]>>7];
			int U=((uvbuf0[i>>1]*uvalpha1+uvbuf1[i>>1]*uvalpha)>>19);
			int V=((uvbuf0[(i>>1)+2048]*uvalpha1+uvbuf1[(i>>1)+2048]*uvalpha)>>19);

			int Cb= yuvtab_40cf[U];
			int Cg= yuvtab_1a1e[V] + yuvtab_0c92[U];
			int Cr= yuvtab_3343[V];

			((uint16_t*)dest)[i] =
				clip_table15b[(Y1 + Cb) >>13] |
				clip_table15g[(Y1 + Cg) >>13] |
				clip_table15r[(Y1 + Cr) >>13];

			((uint16_t*)dest)[i+1] =
				clip_table15b[(Y2 + Cb) >>13] |
				clip_table15g[(Y2 + Cg) >>13] |
				clip_table15r[(Y2 + Cr) >>13];
		}
	}
#endif
}
