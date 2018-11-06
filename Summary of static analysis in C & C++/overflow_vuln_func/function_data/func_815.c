static void
flistOfEventSummaries (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 tag_no, class_tag;
	guint32 lvt;

	if ((*offset) >= tvb_reported_length(tvb))
		return;

	(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	switch (tag_no) {
	case 0:	/
		fObjectIdentifier (tvb, tree, offset, "ObjectId: ");
		break;
	case 1: /
		fApplicationTags (tvb, tree, offset, "eventState: ", bacapp_eventStateFilter);
		break;
	case 2: /
		fApplicationTags (tvb, tree, offset, "acknowledgedTransitions: ", bacapp_EventTransitionBits);
		break;
	case 3: /
		fTimeTag (tvb, tree, offset, "timeStamp: ", lvt);
		(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
		fTimeTag (tvb, tree, offset, "timeStamp: ", lvt);
		(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
		fTimeTag (tvb, tree, offset, "timeStamp: ", lvt);
		break;
	case 4: /
		fApplicationTags (tvb, tree, offset, "NotifyType: ", bacapp_NotifyType);
		break;
	case 5: /
		fApplicationTags (tvb, tree, offset, "eventEnable: ", bacapp_EventTransitionBits);
		break;
	case 6: /
		fUnsignedTag (tvb, tree, offset, "eventPriority: ", lvt);
		(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
		fUnsignedTag (tvb, tree, offset, "eventPriority: ", lvt);
		(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
		fUnsignedTag (tvb, tree, offset, "eventPriority: ", lvt);
		break;
	default:
		return;
		break;
	}
	flistOfEventSummaries (tvb, pinfo, tree, offset);
}
