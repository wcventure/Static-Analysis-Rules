static void
fPrivateTransfer (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 offs, tag_no, class_tag;
	guint32 lvt;

	if ((*offset) >= tvb_reported_length(tvb))
		return;

	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	switch (tag_no) {
	case 0: /
		fUnsignedTag (tvb, tree, offset, "   vendorID: ", lvt);
		break;
	case 1: /
		fUnsignedTag (tvb, tree, offset, "   serviceNumber: ", lvt);
		break;
	case 2: /
		if (!((lvt == 7) && (offs == 0))) {   /
			(*offset) += offs + 1;	/
			proto_tree_add_text(tree, tvb, *offset, max(offs,1), "list of Values {");
			fPropertyValue (tvb, pinfo, tree, offset); /
		} else {
			proto_tree_add_text(tree, tvb, (*offset)++, 1, "}");
		}
		break;
	}
}
