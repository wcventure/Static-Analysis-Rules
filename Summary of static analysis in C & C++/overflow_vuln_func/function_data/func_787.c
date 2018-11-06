static void
fUnsignedTag (tvbuff_t *tvb, proto_tree *tree, guint *offset, guint8 *label, guint32 lvt)
{
	guint8 tmp, i;
	guint64 val = 0;

	(*offset)++;
	for (i = 0; i < min(lvt,8); i++) {
		tmp = tvb_get_guint8(tvb, (*offset)+i);
		val = (val << 8) + tmp;
	}
	proto_tree_add_text(tree, tvb, *offset, min(lvt,8), "%s%" PRIu64, LABEL(label), val);
	(*offset)+=min(lvt,8);
}
