guint32
tvb_get_bits(tvbuff_t *tvb, const gint bit_offset, const gint no_of_bits, const guint encoding _U_)
{
	/
	return (guint32)_tvb_get_bits64(tvb, bit_offset, no_of_bits);
}
