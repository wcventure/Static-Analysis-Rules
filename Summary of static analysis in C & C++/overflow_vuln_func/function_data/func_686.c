gint
tvb_reported_length_remaining(const tvbuff_t *tvb, const gint offset)
{
	guint abs_offset;
	int exception;

	DISSECTOR_ASSERT(tvb && tvb->initialized);

	exception = compute_offset(tvb, offset, &abs_offset);
	if (exception)
		return -1;

	if (tvb->reported_length >= abs_offset)
		return tvb->reported_length - abs_offset;
	else
		return -1;
}
