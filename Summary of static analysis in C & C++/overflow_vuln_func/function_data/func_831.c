static void
fPropertyReference (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 offs, tag_no, class_tag;
	guint32 lvt;

	if ((*offset) >= tvb_reported_length(tvb))
		return;

	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	if (lvt == 7)	/
		return;

	switch (tag_no) {
	case 0:	/
		propertyIdentifier = fPropertyIdentifier (tvb, tree, offset, "property Identifier: ");
		break;
	case 1:	/
		(*offset)+= offs;
		fUnsignedTag (tvb, tree, offset, "propertyArrayIndex: ", lvt);
	break;
	default:
		return;
	}
	fPropertyReference (tvb, pinfo, tree, offset);
}
