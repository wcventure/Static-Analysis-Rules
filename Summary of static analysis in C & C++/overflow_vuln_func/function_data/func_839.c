static void
fReadPropertyConditionalAck (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	guint8 offs, tag_no, class_tag;
	guint32 lvt;

	if ((*offset) >= tvb_reported_length(tvb))
		return;

	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	/
	fReadAccessResult (tvb, pinfo, tree, offset);
	fReadPropertyConditionalAck (tvb, pinfo, tree, offset);
}
