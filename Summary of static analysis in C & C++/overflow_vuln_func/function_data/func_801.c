static void
fWhoHas (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 offs, tag_no, class_tag;
	guint32 lvt;

	if ((*offset) >= tvb_reported_length(tvb))
		return;

	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	switch (tag_no) {
	case 0: /
		fUnsignedTag (tvb, tree, offset, "deviceInstanceLowLimit: ", lvt);
		break;
	case 1: /
		fUnsignedTag (tvb, tree, offset, "deviceInstanceHighLimit: ", lvt);
		break;
	case 2: /
		fObjectIdentifier (tvb, tree, offset, "BACnetObjectId: ");
	break;
	case 3: /
		fApplicationTags (tvb, tree, offset, "ObjectName: ", NULL);
		break;
	default:
		return;
	}
	fWhoHas (tvb, pinfo, tree, offset);
}
