static void
fEventNotification (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 offs, tmp, tag_no, class_tag, i;
	guint32 lvt;
	guint32 val = 0;

	if ((*offset) >= tvb_reported_length(tvb))
		return;

	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	switch (tag_no) {
	case 0:	/
		(*offset) += offs + 1;	/
		for (i = 0; i < min(lvt, 2); i++) {
			tmp = tvb_get_guint8(tvb, (*offset)+i);
			val = (val << 8) + tmp;
		}

		proto_tree_add_uint(tree, hf_bacapp_tag_ProcessId, tvb, *offset, lvt, val);
		(*offset)+=lvt;
		break;
	case 1: /
		fObjectIdentifier (tvb, tree, offset, "initiating DeviceId: ");
	break;
	case 2: /
		fObjectIdentifier (tvb, tree, offset, "event ObjectId: ");
	break;
	case 3:	/
		fApplicationTags (tvb, tree, offset, "Time Stamp: ", NULL);
		break;
	case 4:	/
		fApplicationTags (tvb, tree, offset, "Notification Class: ", NULL);
		break;
	case 5:	/
		fApplicationTags (tvb, tree, offset, "Priority: ", NULL);
		break;
	case 6:	/
		fApplicationTags (tvb, tree, offset, "EventType: ", bacapp_EventType);
		break;
	case 7: /
		fApplicationTags (tvb, tree, offset, "messageText: ", NULL);
		break;
	case 8:	/
		fApplicationTags (tvb, tree, offset, "NotifyType: ", bacapp_NotifyType);
		break;
	case 9: /
		fApplicationTags (tvb, tree, offset, "ackRequired: ", NULL);
		break;
	case 10: /
		fApplicationTags (tvb, tree, offset, "fromState: ", bacapp_EventState);
		break;
	case 11: /
		fApplicationTags (tvb, tree, offset, "toState: ", bacapp_EventState);
		break;
	case 12: /
		fNotificationParameters (tvb, pinfo, tree, offset);
		break;
	default:
		return;
		break;
	}
	fEventNotification (tvb, pinfo, tree, offset);
}
