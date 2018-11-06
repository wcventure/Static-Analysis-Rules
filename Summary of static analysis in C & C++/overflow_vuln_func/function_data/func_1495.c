static void
decode_apn(tvbuff_t *tvb, int offset, guint16 length, proto_tree *tree) {

	gchar	*apn = NULL;
	guint8	name_len, tmp;

	if (length > 0) {
		name_len = tvb_get_guint8 (tvb, offset);

		if (name_len < 0x20) {
			apn = tvb_get_ephemeral_string(tvb, offset + 1, length - 1);
			for (;;) {
				if (name_len >= length - 1) break;
				tmp = name_len;
				name_len = name_len + apn[tmp] + 1;
				apn[tmp] = '.';
			}
		} else
			apn = tvb_get_ephemeral_string(tvb, offset, length);

		proto_tree_add_string (tree, hf_gtp_apn, tvb, offset, length, apn);
	}
}
