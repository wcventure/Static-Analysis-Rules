static void
fAckAlarm (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 offs, tmp, tag_no, class_tag, i;
	guint32 lvt;
	guint32 val = 0;

	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	switch (tag_no) {
	case 0:	/
		fUnsignedTag (tvb, tree, offset, "initiating ObjectId: ", lvt);
		break;
	case 1: /
		fObjectIdentifier (tvb, tree, offset, "initiating ObjectId: ");
	break;
	case 2: /
		fObjectIdentifier (tvb, tree, offset, "monitored ObjectId: ");
	break;
	case 3:	/
		(*offset) += offs + 1;	/
		for (i = 0; i < min(lvt, 4); i++) {
			tmp = tvb_get_guint8(tvb, (*offset)+i);
			val = (val << 8) + tmp;
		}
		proto_tree_add_text(tree, tvb, *offset, lvt, "time remaining (hh.mm.ss): %d.%02d.%02d%s", (int)(val / 3600), (int)((val % 3600) / 60), (int)(val % 60), val == 0 ? " (indefinite)" : "");
		(*offset)+=lvt;
		break;
	case 4:	/
		if (!((lvt == 7) && (offs == 0))) {   /
			(*offset) += offs + 1;	/
			proto_tree_add_text(tree, tvb, *offset, max(offs,1), "list of Values {");
			fPropertyValue (tvb, pinfo, tree, offset); /
		} else {
			proto_tree_add_text(tree, tvb, (*offset)++, 1, "}");
			return;
		}
		break;
	default:
		return;
		break;
	}
	fAckAlarm (tvb, pinfo, tree, offset);
}
