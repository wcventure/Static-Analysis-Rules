static void
fReadPropertyMultipleAck (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 offs, tag_no, class_tag;
	guint32 lvt;

	if ((*offset) >= tvb_reported_length(tvb))
		return;

	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	switch (tag_no) {
	case 0:	/
		fObjectIdentifier (tvb, tree, offset, "BACnetObjectIdentifier: ");
		break;
	case 1:	/
		(*offset) += offs; /
		if ((lvt == 6) && (offs == 0)) {   /
			proto_tree_add_text(tree, tvb, (*offset)++, max(offs,1), "list of Results {");
		}
		if (((lvt == 7) && (offs == 0))) {   /
			proto_tree_add_text(tree, tvb, (*offset)++, 1, "}");
		}
	break;
	case 2:	/
		propertyIdentifier = fPropertyIdentifier (tvb, tree, offset, "property Identifier: ");
		break;
	case 3:	/
		fUnsignedTag (tvb, tree, offset, "PropertyArrayIndex: ", lvt);
		break;
	case 4:	/
		(*offset) += offs; /
		if ((lvt == 6) && (offs == 0)) {   /
			proto_tree_add_text(tree, tvb, (*offset)++, max(offs,1), "list of Values {");
			fPropertyValue (tvb, pinfo, tree, offset); /
		}
		if (((lvt == 7) && (offs == 0))) {   /
			proto_tree_add_text(tree, tvb, (*offset)++, 1, "}");
		}
	break;
	case 5:	/
		(*offset) += offs; /
		if ((lvt == 6) && (offs == 0)) {   /
			proto_tree_add_text(tree, tvb, (*offset)++, max(offs,1), "list of Errors {");
			/
			fApplicationTags (tvb, tree, offset, "   errorClass: ", bacapp_errorClass);
			fApplicationTags (tvb, tree, offset, "   errorCode: ", bacapp_error_code);
		}
		if (((lvt == 7) && (offs == 0))) {   /
			proto_tree_add_text(tree, tvb, (*offset)++, 1, "}");
		}
	break;
	default:
		return;
	}
	fReadPropertyMultipleAck (tvb, pinfo, tree, offset);
}
