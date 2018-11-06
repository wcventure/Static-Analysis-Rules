static void
fReinitializeDevice (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	if ((*offset) >= tvb_reported_length(tvb))
		return;

	/
	*pinfo = *pinfo; /
	*tree = *tree; /

}
