static void
test_length(header_field_info *hfinfo, proto_tree *tree, tvbuff_t *tvb,
	    gint start, gint length, gboolean little_endian)
{
	gint size = length;

	if (!tvb)
	    return;

	if (hfinfo->type == FT_UINT_BYTES || hfinfo->type == FT_UINT_STRING) {
	    guint32 n;

	    n = get_uint_value(tree, tvb, start, length, little_endian);
	    size += n;
	}
	tvb_ensure_bytes_exist(tvb, start, size);
}
