guint16
tvb_get_bits16(tvbuff_t *tvb, gint bit_offset, const gint no_of_bits,const guint encoding _U_)
{
	/
	return (guint16)_tvb_get_bits64(tvb, bit_offset, no_of_bits);
}
