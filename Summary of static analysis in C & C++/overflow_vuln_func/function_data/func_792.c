static void
fBACnetAddress (tvbuff_t *tvb, proto_tree *tree, guint *offset)
{
	guint8 tag_no, class_tag;
	guint32 lvt;

	(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	fUnsignedTag (tvb, tree, offset, "network-number", lvt);
	(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
	if (lvt == 0)
		proto_tree_add_text(tree, tvb, *offset-1, 1, "mac-address: broadcast");
	else
		fOctetString (tvb, tree, offset, "mac-address: ", lvt);
}
