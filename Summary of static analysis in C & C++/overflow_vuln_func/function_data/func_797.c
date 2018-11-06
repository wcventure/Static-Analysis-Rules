static int
fPropertyIdentifier (tvbuff_t *tvb, proto_tree *tree, guint *offset, guint8 *label)
{
	guint8 offs, tag_no, class_tag, tmp, i;
	guint32 lvt;
	guint val = 0;

	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
	(*offset) += offs + 1;

	for (i = 0; i < min(lvt,4); i++) {
		tmp = tvb_get_guint8(tvb, (*offset)+i);
		val = (val << 8) + tmp;
	}
	proto_tree_add_text(tree, tvb, *offset, min(lvt,4),
		"%s%s", LABEL(label),val_to_str(val, bacapp_property_identifier, "(%d) reserved for ASHREA"));
	(*offset)+=min(lvt,4);
	return val;
}
