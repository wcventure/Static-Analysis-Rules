gint
tvb_reported_length_remaining(const tvbuff_t *tvb, const gint offset)
{
	guint abs_offset, abs_length;

	DISSECTOR_ASSERT(tvb && tvb->initialized);

	if (compute_offset_length(tvb, offset, -1, &abs_offset, &abs_length, NULL)) {
		if (tvb->reported_length >= abs_offset)
			return tvb->reported_length - abs_offset;
		else
			return -1;
	}
	else {
		return -1;
	}
}
