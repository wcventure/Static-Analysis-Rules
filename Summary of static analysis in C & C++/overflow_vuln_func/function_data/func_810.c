static void
fAckAlarmRequest (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 tag_no, class_tag;
	guint32 lvt;

	(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	switch (tag_no) {
	case 0:	/
		fUnsignedTag (tvb, tree, offset, "acknowledgingProcessId: ", lvt);
		break;
	case 1: /
		fObjectIdentifier (tvb, tree, offset, "eventObjectId: ");
	break;
	case 2: /
		fApplicationTags (tvb, tree, offset, "eventStateAcknowledged: ", bacapp_EventState);
	break;
	case 3:	/
		fTimeTag (tvb, tree, offset, "timeStamp: ", lvt);
		break;
	case 4:	/
		fApplicationTags (tvb, tree, offset, "acknowledgementSource: ", NULL);
		break;
	case 5:	/
		fTimeTag (tvb, tree, offset, "timeOfAcknowledgement: ", lvt);
		break;
	default:
		return;
		break;
	}
	fAckAlarmRequest (tvb, pinfo, tree, offset);
}
