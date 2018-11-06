static void
fObjectIdentifier (tvbuff_t *tvb, proto_tree *tree, guint *offset, guint8 *label)
{
	guint8 offs, tag_no, class_tag;
	guint32 tmp, val = 0, type;
	guint32 lvt;

	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
	(*offset)+= offs + 1;

	val = tvb_get_guint8(tvb, (*offset));
	tmp = tvb_get_guint8(tvb, (*offset)+1);
	type = (val << 2) + (tmp >> 6);
	val = ((tmp & 0x03) << 16) + (tvb_get_guint8(tvb, (*offset)+2) << 8) + tvb_get_guint8(tvb, (*offset)+3);
	proto_tree_add_text(tree, tvb, *offset, 4,
		"%s%s, instance number %d", LABEL(label), val_to_str(type, bacapp_object_type, "(%d) reserved for ASHREA"), val);
	(*offset)+=4;
}
