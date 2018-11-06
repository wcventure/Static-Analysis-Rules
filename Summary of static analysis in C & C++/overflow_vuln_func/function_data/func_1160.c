static gboolean
compute_offset_length(const tvbuff_t *tvb,
		      const gint offset, const gint length_val,
		      guint *offset_ptr, guint *length_ptr, int *exception)
{
	DISSECTOR_ASSERT(offset_ptr);
	DISSECTOR_ASSERT(length_ptr);

	/
	if (offset >= 0) {
		/
		if ((guint) offset > tvb->reported_length) {
			if (exception) {
				if (tvb->flags & TVBUFF_FRAGMENT) {
					*exception = FragmentBoundsError;
				} else {
					*exception = ReportedBoundsError;
				}
			}
			return FALSE;
		}
		else if ((guint) offset > tvb->length) {
			if (exception) {
				*exception = BoundsError;
			}
			return FALSE;
		}
		else {
			*offset_ptr = offset;
		}
	}
	else {
		/
		if ((guint) -offset > tvb->reported_length) {
			if (exception) {
				if (tvb->flags & TVBUFF_FRAGMENT) {
					*exception = FragmentBoundsError;
				} else {
					*exception = ReportedBoundsError;
				}
			}
			return FALSE;
		}
		else if ((guint) -offset > tvb->length) {
			if (exception) {
				*exception = BoundsError;
			}
			return FALSE;
		}
		else {
			*offset_ptr = tvb->length + offset;
		}
	}

	/
	if (length_val < -1) {
		if (exception) {
			/
			*exception = BoundsError;
		}
		return FALSE;
	}
	else if (length_val == -1) {
		*length_ptr = tvb->length - *offset_ptr;
	}
	else {
		*length_ptr = length_val;
	}

	return TRUE;
}
