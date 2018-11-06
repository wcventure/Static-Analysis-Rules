static const guint8 *
get_field_data(GSList *src_list, field_info *fi)
{
	GSList *src_le;
	data_source *src;
	tvbuff_t *src_tvb;
	gint length, tvbuff_length;

	for (src_le = src_list; src_le != NULL; src_le = src_le->next) {
		src = src_le->data;
		src_tvb = src->tvb;
		if (fi->ds_tvb == src_tvb) {
			/
			length = fi->length;
			tvbuff_length = tvb_length_remaining(src_tvb,
			    fi->start);
			if (length > tvbuff_length)
				length = tvbuff_length;
			return tvb_get_ptr(src_tvb, fi->start, length);
		}
	}
	g_assert_not_reached();
	return NULL;	/
}
