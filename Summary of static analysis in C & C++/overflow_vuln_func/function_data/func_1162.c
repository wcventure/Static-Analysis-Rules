static void
check_offset_length(const tvbuff_t *tvb,
		    const gint offset, gint const length_val,
		    guint *offset_ptr, guint *length_ptr)
{
	int exception = 0;

	if (!check_offset_length_no_exception(tvb,
					      offset, length_val, offset_ptr, length_ptr, &exception)) {
		DISSECTOR_ASSERT(exception > 0);
		THROW(exception);
	}
}
