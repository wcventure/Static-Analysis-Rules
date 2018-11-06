static gboolean
try_get_ber_length(tvbuff_t *tvb, int *bl_offset, guint32 *length, gboolean *ind) {
	int offset = *bl_offset;
	guint8 oct, len;
	guint32 tmp_len;
	gint8 tclass;
	gint32 ttag;
	guint32 tmp_length;
	gboolean tmp_ind;
	int tmp_offset;
	tmp_length = 0;
	tmp_ind = FALSE;

	oct = tvb_get_guint8(tvb, offset);
	offset += 1;

	if(!(oct&0x80)) {
		/
		tmp_length = oct;
	} else {
		len = oct & 0x7F;
		if(len) {
			/
			while (len--) {
				oct = tvb_get_guint8(tvb, offset);
				offset++;
				tmp_length = (tmp_length<<8) + oct;
			}
		} else {
			/
			/	

			tmp_offset = offset;
			
			do {
				tmp_offset = get_ber_identifier(tvb, tmp_offset, &tclass, NULL, &ttag);
				
				try_get_ber_length(tvb, &tmp_offset, &tmp_len, &tmp_ind);

				tmp_offset += tmp_len;

			} while (!((tclass == BER_CLASS_UNI) && (ttag == 0) && (tmp_len == 0))); 

			tmp_length = tmp_offset - offset;
			tmp_ind = TRUE;
		}
	}

	if (length)
		*length = tmp_length;
	if (ind)
		*ind = tmp_ind;

#ifdef DEBUG_BER
printf("get BER length %d, offset %d (remaining %d)\n", tmp_length, offset, tvb_length_remaining(tvb, offset));
#endif

	*bl_offset = offset;
	return TRUE;
}
