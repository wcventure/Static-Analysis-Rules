static void
fNotificationParameters (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 offs, tag_no, class_tag;
	guint32 lvt;

	if ((*offset) >= tvb_reported_length(tvb))
		return;

	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	switch (tag_no) {
	case 0: /
		fApplicationTags (tvb, tree, offset, "referenced-bitstring: ", NULL);
		fApplicationTags (tvb, tree, offset, "status-flags: ", bacapp_statusFlags);
		break;
	case 1: /
		fApplicationTags (tvb, tree, offset, "new-state: ", bacapp_PropertyStates);
		fApplicationTags (tvb, tree, offset, "status-flags: ", bacapp_statusFlags);
		break;
	default:
		return;
	}
	fNotificationParameters (tvb, pinfo, tree, offset);
}
