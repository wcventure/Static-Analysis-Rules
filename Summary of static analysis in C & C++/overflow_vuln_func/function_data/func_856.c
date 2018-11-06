static void
fUnconfirmedTags (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, gint service_choice)
{
	guint8 offs, tag_no, class_tag;
	guint32 lvt;

	if (*offset >= tvb_reported_length(tvb))
		return;

	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	switch (service_choice) {
	case 0:	/
		fIAm (tvb, tree, offset); /
		break;
	case 1: /
		fIHave (tvb, tree, offset); /
	break;
	case 2: /
		fCOVNotification (tvb, pinfo, tree, offset); /
		break;
	case 3: /
		fEventNotification (tvb, pinfo, tree, offset); /
		break;
	case 4: /
		fPrivateTransfer (tvb, pinfo, tree, offset); /
		break;
	case 5: /
		fTextMessage (tvb, pinfo, tree, offset); /
		break;
	case 6: /
		fTimeSynchronization (tvb, tree, offset); /
		break;
	case 7: /
		fWhoHas (tvb, pinfo, tree, offset); /
		break;
	case 8: /
		fWhoIs (tvb, pinfo, tree, offset); /
		break;
	case 9: /
		fUTCTimeSynchronization (tvb, tree, offset); /
		break;
	default:
		break;
	}
}
