static void
add_message_data(tvbuff_t * tvb, gint offset, guint16 data_len,
		 proto_tree * tree)
{
	guint16 *data = NULL;

	if (tree) {
		data = (guint16*)tvb_memcpy(tvb, ep_alloc(data_len), offset, data_len);
	}

	if (data) {
		guint16 *ptr, *end = &data[data_len / 2];
		for (ptr = data; ptr != end; ptr++) {
			/
			gchar *bptr = (gchar *) ptr;
			if (!bptr[0])
				bptr[0] = ' ';

			if (!bptr[1])
				bptr[1] = ' ';

			/
			*ptr = GUINT16_SWAP_LE_BE(*ptr);
		}

		proto_tree_add_string(tree, hf_armagetronad_data, tvb, offset,
				      data_len, (gchar *) data);

		data = NULL;
	} else
		proto_tree_add_item(tree, hf_armagetronad_data, tvb, offset,
				    data_len, FALSE);
}
