static void
fAtomicReadFile (tvbuff_t *tvb, proto_tree *tree, guint *offset)
{
	guint8 offs, tag_no, class_tag;
	guint32 lvt;

	if ((bacapp_flags & 0x08) && (bacapp_seq != 0)) {	/
		if (bacapp_flags & 0x04) { /
			fOctetString (tvb, tree, offset, "   fileData: ", 0);
		} else {
			fOctetString (tvb, tree, offset, "   fileData: ", tvb_reported_length(tvb) - *offset - 1);
			(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
			if (lvt == 7) {   /
				proto_tree_add_text(tree, tvb, (*offset)++, 1, "}");
			}
		}
	} else {
		fApplicationTags (tvb, tree, offset, "EndOfFile: ", NULL);

		offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

		switch (tag_no) {
		case 0:	/
			(*offset) += offs; /
			if ((lvt == 6) && (offs == 0)) {   /
				proto_tree_add_text(tree, tvb, (*offset)++, max(offs,1), "streamAccess {");
			}
			(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
			fSignedTag (tvb, tree, offset, "   FileStartPosition: ", lvt);
			fApplicationTags (tvb, tree, offset, "   fileData: ", NULL);
			if (bacapp_flags && 0x04) { /
				break;
			}
			(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
			if (((lvt == 7) && (offs == 0))) {   /
				proto_tree_add_text(tree, tvb, (*offset)++, 1, "}");
			}
			break;
		case 1:	/
			(*offset) += offs; /
			if ((lvt == 6) && (offs == 0)) {   /
				proto_tree_add_text(tree, tvb, (*offset)++, max(offs,1), "streamAccess {");
			}
			(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
			fSignedTag (tvb, tree, offset, "  FileStartRecord: ", lvt);
			fUnsignedTag (tvb, tree, offset, "  returnedRecordCount: ", lvt);
			fApplicationTags (tvb, tree, offset, "  Data: ", NULL);
			if (bacapp_flags && 0x04) { /
				break;
			}
			(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
			if (((lvt == 7) && (offs == 0))) {   /
				proto_tree_add_text(tree, tvb, (*offset)++, 1, "}");
			}
		break;
		default:
			return;
		}
	}
}
