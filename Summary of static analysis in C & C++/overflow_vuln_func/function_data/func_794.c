static void
fRecipient (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 tag_no, class_tag;
	guint32 lvt;

	(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	switch (tag_no) {
	case 0:	/
		fObjectIdentifier (tvb, tree, offset, "device: ");
		break;
	case 1:	/
		fBACnetAddress (tvb, tree, offset);
		break;
	default:
		return;
		break;
	}
	fRecipient (tvb, pinfo, tree, offset);
}
