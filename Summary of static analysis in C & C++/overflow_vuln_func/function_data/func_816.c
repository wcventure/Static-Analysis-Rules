static void
fGetEventInformation (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 tag_no, class_tag;
	guint32 lvt;

	if ((*offset) >= tvb_reported_length(tvb))
		return;

	(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	switch (tag_no) {
	case 0:	/
		flistOfEventSummaries (tvb, pinfo, tree, offset);
		break;
	case 1: /
		fApplicationTags (tvb, tree, offset, "moreEvents: ", NULL);
		break;
	default:
		return;
		break;
	}
	fGetEventInformationRequest (tvb, pinfo, tree, offset);
}
