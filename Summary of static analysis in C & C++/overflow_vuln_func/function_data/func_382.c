static gboolean
check_for_undecoded(proto_node *node, gpointer data)
{
	field_info *fi = PNODE_FINFO(node);
	gchar* decoded = (gchar*)data;
	gint i;
	guint byte;
	guint bit;

	if (fi && fi->hfinfo->type != FT_PROTOCOL) {
		for (i = fi->start; i < fi->start + fi->length; i++) {
			byte = i / 8;
			bit = i % 8;
			decoded[byte] |= (1 << bit);
		}
	}

	return FALSE;
}
