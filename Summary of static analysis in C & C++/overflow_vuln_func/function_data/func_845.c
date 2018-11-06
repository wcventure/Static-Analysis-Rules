static void
fAtomicReadFileRequest (tvbuff_t *tvb, proto_tree *tree, guint *offset)
{
	guint8 offs, tag_no, class_tag;
	guint32 lvt;

	fObjectIdentifier (tvb, tree, offset, "BACnetObjectIdentifier: ");

	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	switch (tag_no) {
	case 0:	/
		(*offset) += offs; /
		if ((lvt == 6) && (offs == 0)) {   /
			proto_tree_add_text(tree, tvb, (*offset)++, max(offs,1), "streamAccess {");
		}
		(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
		fSignedTag (tvb, tree, offset, "   FileStartPosition: ", lvt);
		(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
		fUnsignedTag (tvb, tree, offset, "   requestetOctetCount: ", lvt);
		(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
		if (((lvt == 7) && (offs == 0))) {   /
			proto_tree_add_text(tree, tvb, (*offset)++, 1, "}");
		}
		break;
	case 1:	/
		(*offset) += offs; /
		if ((lvt == 6) && (offs == 0)) {   /
			proto_tree_add_text(tree, tvb, (*offset)++, max(offs,1), "recordAccess {");
		}
		(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
		fSignedTag (tvb, tree, offset, "   FileStartRecord: ", lvt);
		(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
		fUnsignedTag (tvb, tree, offset, "   requestetRecordCount: ", lvt);
		(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
		if (((lvt == 7) && (offs == 0))) {   /
			proto_tree_add_text(tree, tvb, (*offset)++, 1, "}");
		}
	break;
	default:
		return;
	}
}
