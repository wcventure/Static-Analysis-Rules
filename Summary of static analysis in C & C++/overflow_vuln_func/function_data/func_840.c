static void
fObjectSpecifier (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 offs, tag_no, class_tag;
	guint32 lvt;

	if ((*offset) >= tvb_reported_length(tvb))
		return;

	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	switch (tag_no) {
	case 0:	/
		proto_tree_add_item(tree, hf_bacapp_tag_initiatingObjectType, tvb, (*offset), 1, TRUE);
	break;
	case 1:	/
		fObjectIdentifier (tvb, tree, offset, "BACnetObjectIdentifier: ");
	break;
	}
	fObjectSpecifier (tvb, pinfo, tree, offset);
}
