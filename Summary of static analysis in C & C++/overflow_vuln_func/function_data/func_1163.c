gint
tvb_length_remaining(const tvbuff_t *tvb, const gint offset)
{
	guint abs_offset, abs_length;

	DISSECTOR_ASSERT(tvb && tvb->initialized);

	if (compute_offset_length(tvb, offset, -1, &abs_offset, &abs_length, NULL)) {
		return abs_length;
	}
	else {
		return -1;
	}
}
