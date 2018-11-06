static void
fReadRangeRequest (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
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
		propertyIdentifier = fPropertyIdentifier (tvb, tree, offset, "property Identifier: ");
		break;
	case 2:	/
		fUnsignedTag (tvb, tree, offset, "PropertyArrayIndex: ", lvt);
		break;
	case 3:	/
		(*offset) += offs; /
		if ((lvt == 6) && (offs == 0)) {   /
			proto_tree_add_text(tree, tvb, (*offset)++, max(offs,1), "range byPosition: referenceIndex, count {");
			fPropertyValue (tvb, pinfo, tree, offset); /
		}
		if (((lvt == 7) && (offs == 0))) {   /
			proto_tree_add_text(tree, tvb, (*offset)++, 1, "}");
			return;
		}
	break;
	case 4:	/
		(*offset) += offs; /
		if ((lvt == 6) && (offs == 0)) {   /
			proto_tree_add_text(tree, tvb, (*offset)++, max(offs,1), "range byTime: referenceTime, count {");
			fPropertyValue (tvb, pinfo, tree, offset); /
		}
		if (((lvt == 7) && (offs == 0))) {   /
			proto_tree_add_text(tree, tvb, (*offset)++, 1, "}");
			return;
		}
	break;
	case 5:	/
		(*offset) += offs; /
		if ((lvt == 6) && (offs == 0)) {   /
			proto_tree_add_text(tree, tvb, (*offset)++, max(offs,1), "TimeRange: beginningTime, endingTime {");
			fPropertyValue (tvb, pinfo, tree, offset); /
		}
		if (((lvt == 7) && (offs == 0))) {   /
			proto_tree_add_text(tree, tvb, (*offset)++, 1, "}");
			return;
		}
	break;
	default:
		return;
	}
	fReadRangeRequest (tvb, pinfo, tree, offset);
}
