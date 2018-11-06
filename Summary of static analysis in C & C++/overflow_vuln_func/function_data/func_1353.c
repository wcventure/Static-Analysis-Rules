static gint
_tvb_get_nstringz(tvbuff_t *tvb, gint offset, guint maxlength, guint8* buffer,
		gint *bytes_copied)
{
	gint	stringlen;
	guint	abs_offset, junk_length;
	gint	limit, len;
	gboolean decreased_max = FALSE;

	check_offset_length(tvb, offset, 0, &abs_offset, &junk_length);

	if (maxlength == 0) {
		buffer[0] = 0;
		return 0;
	}

	/
	len = tvb_length_remaining(tvb, abs_offset);

	/
	if (len == 0) {
		THROW(ReportedBoundsError);
	}

	/
	g_assert(len != -1);

	if ((guint)len < maxlength) {
		limit = len;
		decreased_max = TRUE;
	}
	else {
		limit = maxlength;
	}

	stringlen = tvb_strnlen(tvb, abs_offset, limit);
	/
	if (stringlen == -1) {
		tvb_memcpy(tvb, buffer, abs_offset, limit);
		if (decreased_max) {
			buffer[limit] = 0;
			/
			*bytes_copied = limit + 1;
		}
		else {
			*bytes_copied = limit;
		}
		return -1;
	}

	/
	tvb_memcpy(tvb, buffer, abs_offset, stringlen + 1);
	*bytes_copied = stringlen + 1;
	return stringlen;
}
