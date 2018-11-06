gboolean
tvb_bytes_exist(const tvbuff_t *tvb, const gint offset, const gint length)
{
	guint abs_offset, abs_length;

	DISSECTOR_ASSERT(tvb && tvb->initialized);

	if (!compute_offset_length(tvb, offset, length, &abs_offset, &abs_length, NULL))
		return FALSE;

	if (abs_offset + abs_length <= tvb->length) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}
