static void
fPropertyValue (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 offs, tag_no, class_tag;
	guint32 lvt;
	static int awaitingClosingTag = 0;

	if ((*offset) >= tvb_reported_length(tvb))
		return;

	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	if (((lvt == 7) && (offs == 0)) && !awaitingClosingTag) {  /
		return;	/
	}

	if (class_tag) {
		switch (tag_no) {
		case 0:	/
			propertyIdentifier = fPropertyIdentifier (tvb, tree, offset,  "   property Identifier: ");
			break;
		case 1:	/
			fPropertyIdentifier (tvb, tree, offset, "propertyArrayIndex: ");
		break;
		case 2:  /
			(*offset) += offs + 1;	/
			if ((lvt == 6) && (offs == 0)) {   /
				awaitingClosingTag = 1;
				if (propertyIdentifier == 111) {	/
					fApplicationTags (tvb, tree, offset, "   propertyValue: ", bacapp_statusFlags);
				} else {
					fApplicationTags (tvb, tree, offset, NULL, NULL);
				}
			}
			if (((lvt == 7) && (offs == 0)))  /
				awaitingClosingTag = 0; /
			break;
		case 3:  /
			(*offset) += offs;	/
			fSignedTag (tvb, tree, offset, "   Priority: ", lvt);
		break;
		default:
			break;
		}
	} else {
		switch (propertyIdentifier)
		{
		case 97: /
			fApplicationTags (tvb, tree, offset, "   propertyValue: ", bacapp_servicesSupported);
			break;
		case 111: /
			fApplicationTags (tvb, tree, offset, "   propertyValue: ", bacapp_statusFlags);
			break;
		case 76:  /
			fApplicationTags (tvb, tree, offset, "   propertyValue: ", NULL);
			break;
		default:
			fApplicationTags (tvb, tree, offset, "   propertyValue: ", NULL);
			break;
		}
	}
	fPropertyValue (tvb, pinfo, tree, offset);
}
