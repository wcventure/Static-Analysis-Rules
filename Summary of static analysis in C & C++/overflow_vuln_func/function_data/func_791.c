static void
fOctetString (tvbuff_t *tvb, proto_tree *tree, guint *offset, guint8 *label, guint32 lvt)
{
	guint8 *str_val;
	guint32 len;

	if ((lvt == 0) || (lvt > tvb_reported_length(tvb)))
		lvt = tvb_reported_length(tvb) - *offset;

	proto_tree_add_text(tree, tvb, *offset, (int)lvt, "[displayed OctetString with %u Bytes:] %s", lvt, LABEL(label));

	do {
		len = min (lvt, 200);
		str_val = tvb_get_string(tvb, *offset, len);
/
		proto_tree_add_text(tree, tvb, *offset, len, "%s", str_val);
		g_free(str_val);
		lvt -= len;
		(*offset) += len;
	} while (lvt > 0);
}
