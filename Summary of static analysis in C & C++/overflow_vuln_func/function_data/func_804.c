static void
fTextMessage (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 tag_no, class_tag;
	guint32 lvt;

	if ((*offset) >= tvb_reported_length(tvb))
		return;

	(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	switch (tag_no) {
	case 0:	/
		fObjectIdentifier (tvb, tree, offset, "TextMessageSourceDevice: ");
		break;
	case 1: /
		(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
		switch (tag_no) {
		case 0: /
			fUnsignedTag (tvb, tree, offset, "   messageClass: ", lvt);
			break;
		case 1: /
			fApplicationTags (tvb, tree, offset, "messageClass: ", NULL);
			break;
		}
		break;
	case 2: /
		fApplicationTags (tvb, tree, offset, "ObjectName: ", bacapp_messagePriority);
		break;
	case 3: /
		fApplicationTags (tvb, tree, offset, "message: ", NULL);
		break;
	}
	fTextMessage (tvb, pinfo, tree, offset);
}
