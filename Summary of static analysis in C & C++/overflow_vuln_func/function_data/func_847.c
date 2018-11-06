static void
fAtomicWriteFileAck (tvbuff_t *tvb, proto_tree *tree, guint *offset)
{
	guint8 tag_no, class_tag;
	guint32 lvt;

	(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	switch (tag_no) {
	case 0:	/
		fSignedTag (tvb, tree, offset, "   FileStartPosition: ", lvt);
		break;
	case 1:	/
		fSignedTag (tvb, tree, offset, "  fileStartRecord: ", lvt);
	break;
	default:
		return;
	}
}
