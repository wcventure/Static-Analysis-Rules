static void
test_length(header_field_info *hfinfo, proto_tree *tree, tvbuff_t *tvb,
	    gint start, gint length, const guint encoding)
{
	gint size = length;

	if (!tvb)
		return;

	if (hfinfo->type == FT_UINT_BYTES || hfinfo->type == FT_UINT_STRING) {
		guint32 n;

		n = get_uint_value(tree, tvb, start, length, encoding);
		if (n > size + n) {
			/
			size = -1;
		}
		else {
			size += n;
		}
	} else if (hfinfo->type == FT_STRINGZ) {
		/
		if (length == -1)
			size = 0;
	}

	tvb_ensure_bytes_exist(tvb, start, size);
}
