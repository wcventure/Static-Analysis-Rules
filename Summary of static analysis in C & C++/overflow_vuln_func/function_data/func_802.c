static void
fUTCTimeSynchronization (tvbuff_t *tvb, proto_tree *tree, guint *offset)
{
	guint8 tag_no, class_tag;
	guint32 lvt;

	if ((*offset) >= tvb_reported_length(tvb))
		return;

	(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	fDateTag (tvb, tree, offset, "Date: ", lvt);
	fTimeTag (tvb, tree, offset, "UTC-Time: ", lvt);
}
