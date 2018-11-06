guint
tvb_ensure_length_remaining(const tvbuff_t *tvb, const gint offset)
{
	guint abs_offset, abs_length;
	int   exception;

	DISSECTOR_ASSERT(tvb && tvb->initialized);

	if (!compute_offset_length(tvb, offset, -1, &abs_offset, &abs_length, &exception)) {
		THROW(exception);
	}
	if (abs_length == 0) {
		/
		if (abs_offset >= tvb->reported_length) {
			if (tvb->flags & TVBUFF_FRAGMENT) {
				THROW(FragmentBoundsError);
			} else {
				THROW(ReportedBoundsError);
			}
		} else
			THROW(BoundsError);
	}
	return abs_length;
}
