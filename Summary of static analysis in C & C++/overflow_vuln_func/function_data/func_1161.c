static gboolean
check_offset_length_no_exception(const tvbuff_t *tvb,
				 const gint offset, gint const length_val,
				 guint *offset_ptr, guint *length_ptr, int *exception)
{
	guint	end_offset;

	if (!compute_offset_length(tvb,
				   offset, length_val, offset_ptr, length_ptr, exception)) {
		return FALSE;
	}

	/
	end_offset = *offset_ptr + *length_ptr;

	/
	if (end_offset < *offset_ptr)
		end_offset = UINT_MAX;

	/
	if (end_offset <= tvb->length) {
		return TRUE;
	}
	else {
		if (exception) {
			if (end_offset <= tvb->reported_length) {
				*exception = BoundsError;
			}
			else {
				if (tvb->flags & TVBUFF_FRAGMENT) {
					*exception = FragmentBoundsError;
				} else {
					*exception = ReportedBoundsError;
				}
			}
		}

		return FALSE;
	}
}
