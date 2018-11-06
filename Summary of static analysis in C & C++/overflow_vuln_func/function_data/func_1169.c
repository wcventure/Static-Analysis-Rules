static const guint8*
ensure_contiguous_no_exception(tvbuff_t *tvb, const gint offset, const gint length, int *exception)
{
	guint abs_offset, abs_length;

	if (!check_offset_length_no_exception(tvb, offset, length,
		&abs_offset, &abs_length, exception)) {
		return NULL;
	}

	/
	if (tvb->real_data)
		return tvb->real_data + abs_offset;


	if (tvb->ops->tvb_get_ptr)
		return tvb->ops->tvb_get_ptr(tvb, abs_offset, abs_length);

	DISSECTOR_ASSERT_NOT_REACHED();
	return NULL;
}
