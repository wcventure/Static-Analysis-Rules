guint8 *
ep_tvb_get_bits(tvbuff_t *tvb, gint bit_offset, gint no_of_bits, gboolean lsb0)
{
	gint    no_of_bytes;
	guint8 *buf;

	/

	if (no_of_bits < 0 || bit_offset < 0) {
		DISSECTOR_ASSERT_NOT_REACHED();
	}

	no_of_bytes = (no_of_bits >> 3) + ((no_of_bits & 0x7) != 0);	/
	buf         = ep_alloc(no_of_bytes);
	tvb_get_bits_buf(tvb, bit_offset, no_of_bits, buf, lsb0);
	return buf;
}
