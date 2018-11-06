tvbuff_t *
tvb_uncompress(tvbuff_t *tvb, const int offset, int comprlen)
{
	gint       err;
	guint      bytes_out      = 0;
	guint8    *compr;
	guint8    *uncompr        = NULL;
	tvbuff_t  *uncompr_tvb    = NULL;
	z_streamp  strm;
	Bytef     *strmbuf;
	guint      inits_done     = 0;
	gint       wbits          = MAX_WBITS;
	guint8    *next;
	guint      bufsiz;
#ifdef TVB_Z_DEBUG
	guint      inflate_passes = 0;
	guint      bytes_in       = tvb_captured_length_remaining(tvb, offset);
#endif

	if (tvb == NULL) {
		return NULL;
	}

	compr = (guint8 *)tvb_memdup(NULL, tvb, offset, comprlen);

	if (!compr)
		return NULL;

	/
	bufsiz = tvb_captured_length_remaining(tvb, offset) * 2;
	bufsiz = CLAMP(bufsiz, TVB_Z_MIN_BUFSIZ, TVB_Z_MAX_BUFSIZ);

#ifdef TVB_Z_DEBUG
	printf("bufsiz: %u bytes\n", bufsiz);
#endif

	next = compr;

	strm            = g_new0(z_stream, 1);
	strm->next_in   = next;
	strm->avail_in  = comprlen;

	strmbuf         = (Bytef *)g_malloc0(bufsiz);
	strm->next_out  = strmbuf;
	strm->avail_out = bufsiz;

	err = inflateInit2(strm, wbits);
	inits_done = 1;
	if (err != Z_OK) {
		inflateEnd(strm);
		g_free(strm);
		g_free(compr);
		g_free(strmbuf);
		return NULL;
	}

	while (1) {
		memset(strmbuf, '\0', bufsiz);
		strm->next_out  = strmbuf;
		strm->avail_out = bufsiz;

		err = inflate(strm, Z_SYNC_FLUSH);

		if (err == Z_OK || err == Z_STREAM_END) {
			guint bytes_pass = bufsiz - strm->avail_out;

#ifdef TVB_Z_DEBUG
			++inflate_passes;
#endif

			if (uncompr == NULL) {
				/
				uncompr = (guint8 *)((bytes_pass || err != Z_STREAM_END) ?
						g_memdup(strmbuf, bytes_pass) :
						g_strdup(""));
			} else {
				guint8 *new_data = (guint8 *)g_malloc0(bytes_out + bytes_pass);

				memcpy(new_data, uncompr, bytes_out);
				memcpy(new_data + bytes_out, strmbuf, bytes_pass);

				g_free(uncompr);
				uncompr = new_data;
			}

			bytes_out += bytes_pass;

			if (err == Z_STREAM_END) {
				inflateEnd(strm);
				g_free(strm);
				g_free(strmbuf);
				break;
			}
		} else if (err == Z_BUF_ERROR) {
			/
			inflateEnd(strm);
			g_free(strm);
			g_free(strmbuf);

			if (uncompr != NULL) {
				break;
			} else {
				g_free(compr);
				return NULL;
			}

		} else if (err == Z_DATA_ERROR && inits_done == 1
			&& uncompr == NULL && comprlen >= 2 &&
			(*compr  == 0x1f) && (*(compr + 1) == 0x8b)) {
			/

			/
			Bytef *c = compr + 2;
			Bytef  flags = 0;

			/
			if (comprlen < 10 || *c != Z_DEFLATED) {
				inflateEnd(strm);
				g_free(strm);
				g_free(compr);
				g_free(strmbuf);
				return NULL;
			}

			c++;
			flags = *c;
			c++;

			/
			c += 6;

			if (flags & (1 << 2)) {
				/
				guint16 xsize = 0;

				if (c-compr < comprlen) {
					xsize += *c;
					c++;
				}
				if (c-compr < comprlen) {
					xsize += *c << 8;
					c++;
				}

				c += xsize;
			}

			if (flags & (1 << 3)) {
				/

				while ((c - compr) < comprlen && *c != '\0') {
					c++;
				}

				c++;
			}

			if (flags & (1 << 4)) {
				/

				while ((c - compr) < comprlen && *c != '\0') {
					c++;
				}

				c++;
			}


			inflateReset(strm);
			next = c;
			strm->next_in = next;
			if (c - compr > comprlen) {
				inflateEnd(strm);
				g_free(strm);
				g_free(compr);
				g_free(strmbuf);
				return NULL;
			}
			comprlen -= (int) (c - compr);

			inflateEnd(strm);
			inflateInit2(strm, wbits);
			inits_done++;
		} else if (err == Z_DATA_ERROR && uncompr == NULL &&
			inits_done <= 3) {

			/
			wbits = -MAX_WBITS;

			inflateReset(strm);

			strm->next_in   = next;
			strm->avail_in  = comprlen;

			inflateEnd(strm);
			memset(strmbuf, '\0', bufsiz);
			strm->next_out  = strmbuf;
			strm->avail_out = bufsiz;

			err = inflateInit2(strm, wbits);

			inits_done++;

			if (err != Z_OK) {
				g_free(strm);
				g_free(strmbuf);
				g_free(compr);
				g_free(uncompr);

				return NULL;
			}
		} else {
			inflateEnd(strm);
			g_free(strm);
			g_free(strmbuf);

			if (uncompr == NULL) {
				g_free(compr);
				return NULL;
			}

			break;
		}
	}

#ifdef TVB_Z_DEBUG
	printf("inflate() total passes: %u\n", inflate_passes);
	printf("bytes  in: %u\nbytes out: %u\n\n", bytes_in, bytes_out);
#endif

	if (uncompr != NULL) {
		uncompr_tvb =  tvb_new_real_data((guint8*) uncompr, bytes_out, bytes_out);
		tvb_set_free_cb(uncompr_tvb, g_free);
	}
	g_free(compr);
	return uncompr_tvb;
}
