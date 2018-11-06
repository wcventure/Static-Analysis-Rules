static void
fSubscribeCOV (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
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
		for (i = 0; i < (guint8) min(lvt, 2); i++) {
			tmp = tvb_get_guint8(tvb, (*offset)+i);
			val = (val << 8) + tmp;
		}

		proto_tree_add_uint(tree, hf_bacapp_tag_ProcessId, tvb, *offset, lvt, val);
		(*offset)+=lvt;
		break;
	case 1: /
		fObjectIdentifier (tvb, tree, offset, "monitored ObjectId: ");
	break;
	case 2: /
		fApplicationTags (tvb, tree, offset, "issueConfirmedNotifications: ", NULL);
		break;
	case 3:	/
		(*offset) += offs + 1;	/
		for (i = 0; i < (guint8) min(lvt, 4); i++) {
			tmp = tvb_get_guint8(tvb, (*offset)+i);
			val = (val << 8) + tmp;
		}
		proto_tree_add_text(tree, tvb, *offset, lvt, "life time (hh.mm.ss): %d.%02d.%02d%s", (int)(val / 3600), (int)((val % 3600) / 60), (int)(val % 60), val == 0 ? " (indefinite)" : "");
		(*offset)+=lvt;
		return;
		break;
	default:
		return;
		break;
	}
	fSubscribeCOV (tvb, pinfo, tree, offset);
}
