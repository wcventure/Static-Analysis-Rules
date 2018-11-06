static int decode_frame(AVCodecContext *avctx, void *data, int *data_size, uint8_t *buf, int buf_size)
{
	EightBpsContext * const c = (EightBpsContext *)avctx->priv_data;
	unsigned char *encoded = (unsigned char *)buf;
	unsigned char *pixptr;
	unsigned int height = avctx->height; // Real image height
	unsigned int dlen, p, row;
	unsigned char *lp, *dp;
	unsigned char count;
	unsigned int px_inc;
	unsigned int planes = c->planes;
	unsigned char *planemap = c->planemap;
  
  
	/
	if (buf_size == 0)
		return 0;

	if(c->pic.data[0])
		avctx->release_buffer(avctx, &c->pic);

	c->pic.reference = 0;
	c->pic.buffer_hints = FF_BUFFER_HINTS_VALID;
	if(avctx->get_buffer(avctx, &c->pic) < 0){
		av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
		return -1;
	}

	/
	dp = encoded + planes * (height << 1);

	/
	if (planes == 4)
		planes--;

	px_inc = planes + (avctx->pix_fmt == PIX_FMT_RGBA32);

	for (p = 0; p < planes; p++) {
		/
		lp = encoded + p * (height << 1);

		/
		for(row = 0; row < height; row++) {
			pixptr = c->pic.data[0] + row * c->pic.linesize[0] + planemap[p];
			dlen = be2me_16(*(unsigned short *)(lp+row*2));
			/
			while(dlen > 0) {
				if ((count = *dp++) <= 127) {
					count++;
					dlen -= count + 1;
					while(count--) {
						*pixptr = *dp++;
						pixptr += px_inc;
					}
				} else {
					count = 257 - count;
					while(count--) {
						*pixptr = *dp;
						pixptr += px_inc;
					}
					dp++;
					dlen -= 2;
				}
			}
		}
	}

	if (avctx->palctrl) {
		memcpy (c->pic.data[1], avctx->palctrl->palette, AVPALETTE_SIZE);
		if (avctx->palctrl->palette_changed) {
			c->pic.palette_has_changed = 1;
			avctx->palctrl->palette_changed = 0;
		} else
			c->pic.palette_has_changed = 0;
	}

	*data_size = sizeof(AVFrame);
	*(AVFrame*)data = c->pic;

	/
	return buf_size;
}
