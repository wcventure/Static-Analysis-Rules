static void
fgetEnrollmentSummaryRequest (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 tag_no, class_tag;
	guint32 lvt;

	if ((*offset) >= tvb_reported_length(tvb))
		return;

	(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	switch (tag_no) {
	case 0:	/
		fApplicationTags (tvb, tree, offset, "acknowledgmentFilter: ", bacapp_AcknowledgementFilter);
		break;
	case 1: /
		fRecipientProcess (tvb, pinfo, tree, offset);
	break;
	case 2: /
		fApplicationTags (tvb, tree, offset, "eventStateFilter: ", bacapp_eventStateFilter);
	break;
	case 3:	/
		fApplicationTags (tvb, tree, offset, "eventTypeFilter: ", bacapp_EventType);
		break;
	case 4:	/
		(*offset)++;
		fUnsignedTag (tvb, tree, offset, "minPriority: ", lvt);
		(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
		fUnsignedTag (tvb, tree, offset, "maxPriority: ", lvt);
		break;
	case 5:	/
		fUnsignedTag (tvb, tree, offset, "notificationClassFilter: ", lvt);
		break;
	default:
		return;
		break;
	}
	fgetEnrollmentSummaryRequest (tvb, pinfo, tree, offset);
}
