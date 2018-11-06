guint8
tvb_get_bits8(tvbuff_t *tvb, gint bit_offset, const gint no_of_bits)
{
	return (guint8)_tvb_get_bits64(tvb, bit_offset, no_of_bits);
}
