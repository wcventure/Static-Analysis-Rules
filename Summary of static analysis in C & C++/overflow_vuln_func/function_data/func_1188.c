static int
try_get_ber_length(tvbuff_t *tvb, int offset, guint32 *length, gboolean *ind, gint nest_level) {
	guint8 oct, len;
	guint32 indef_len;
	guint32 tmp_length;
	gboolean tmp_ind;
	int tmp_offset,s_offset;
	gint8 tclass;
	gboolean tpc;
	gint32 ttag;
	tmp_length = 0;
	tmp_ind = FALSE;

	if (nest_level > BER_MAX_INDEFINITE_NESTING) {
		/
		THROW(ReportedBoundsError);
	}

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

			tmp_offset = offset;
			/
			/
			/
			while (tvb_get_guint8(tvb, offset) || tvb_get_guint8(tvb, offset+1)) {
				/
				s_offset=offset;
				offset= get_ber_identifier(tvb, offset, &tclass, &tpc, &ttag);
				offset= try_get_ber_length(tvb,offset, &indef_len, NULL, nest_level+1);
				tmp_length += indef_len+(offset-s_offset); /
				offset += indef_len;
                                /
				if (offset <= s_offset)
					THROW(ReportedBoundsError);
			}
			tmp_length += 2;
			tmp_ind = TRUE;
			offset = tmp_offset;
		}
	}

	if (length)
		*length = tmp_length;
	if (ind)
		*ind = tmp_ind;

#ifdef DEBUG_BER
printf("get BER length %d, offset %d (remaining %d)\n", tmp_length, offset, tvb_length_remaining(tvb, offset));
#endif

	return offset;
}
