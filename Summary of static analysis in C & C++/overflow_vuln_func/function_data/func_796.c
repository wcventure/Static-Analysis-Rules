static void
fBACnetAddressBinding (tvbuff_t *tvb, proto_tree *tree, guint *offset)
{
	guint8 tag_no, class_tag;
	guint32 lvt;

	(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	fObjectIdentifier (tvb, tree, offset, "deviceObjectId: ");
	(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
	fBACnetAddress (tvb, tree, offset);
}
