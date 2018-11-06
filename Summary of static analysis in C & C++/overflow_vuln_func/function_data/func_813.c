static void
fgetEnrollmentSummaryAck (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 tag_no, class_tag;
	guint32 lvt;

	if ((*offset) >= tvb_reported_length(tvb))
		return;

	fObjectIdentifier (tvb, tree, offset, "ObjectId: ");
	fApplicationTags (tvb, tree, offset, "eventType: ", bacapp_EventType);
	fApplicationTags (tvb, tree, offset, "eventState: ", bacapp_eventStateFilter);
	(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
	fUnsignedTag (tvb, tree, offset, "Priority: ", lvt);
	(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
	fUnsignedTag (tvb, tree, offset, "notificationClass: ", lvt);

	fgetEnrollmentSummaryAck (tvb, pinfo, tree, offset);
}
