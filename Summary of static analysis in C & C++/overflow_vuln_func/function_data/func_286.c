static gboolean
check_offset_length_no_exception(tvbuff_t *tvb, gint offset, gint length,
		guint *offset_ptr, guint *length_ptr, int *exception)
{
	g_assert(tvb->initialized);

	if (!compute_offset_length(tvb, offset, length, offset_ptr, length_ptr, exception)) {
		return FALSE;
	}

	if (*offset_ptr + *length_ptr <= tvb->length) {
		return TRUE;
	}
	else if (*offset_ptr + *length_ptr <= tvb->reported_length) {
		if (exception) {
			*exception = BoundsError;
		}
		return FALSE;
	}
	else {
		if (exception) {
			*exception = ReportedBoundsError;
		}
		return FALSE;
	}

	g_assert_not_reached();
}
