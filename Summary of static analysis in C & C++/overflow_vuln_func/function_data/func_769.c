static void
dissect_vendor_ie_rsn(proto_tree * ietree, proto_tree * tree, tvbuff_t * tvb,
	int offset, guint32 tag_len, const guint8 *tag_val)
{
	guint32 tag_val_off = 0;
	char out_buff[SHORT_STR], *pos;
	guint i;

	if (tag_val_off + 4 <= tag_len && !memcmp(tag_val, RSN_OUI"\x04", 4)) {
		/
		pos = out_buff;
		pos += g_snprintf(pos, out_buff + SHORT_STR - pos, "RSN PMKID: ");
		if (tag_len - 4 != PMKID_LEN) {
			pos += g_snprintf(pos, out_buff + SHORT_STR - pos,
				"(invalid PMKID len=%d, expected 16) ", tag_len - 4);
		}
		for (i = 0; i < tag_len - 4; i++) {
			pos += g_snprintf(pos, out_buff + SHORT_STR - pos, "%02X",
				tag_val[tag_val_off + 4 + i]);
		}
		proto_tree_add_string(tree, tag_interpretation, tvb, offset,
			tag_len, out_buff);
	}
	proto_item_append_text(ietree, ": RSN");
}
