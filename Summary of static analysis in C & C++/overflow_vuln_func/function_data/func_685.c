gint
tvb_length_remaining(const tvbuff_t *tvb, const gint offset)
{
	guint abs_offset, rem_length;
	int exception;

	DISSECTOR_ASSERT(tvb && tvb->initialized);

	exception = compute_offset_and_remaining(tvb, offset, &abs_offset, &rem_length);
	if (exception)
		return -1;

	return rem_length;
}
