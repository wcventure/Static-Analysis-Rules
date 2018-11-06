static void
fGetAlarmSummary (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
	if ((*offset) >= tvb_reported_length(tvb))
		return;

	fObjectIdentifier (tvb, tree, offset, "objectIdentifier: ");
	fApplicationTags (tvb, tree, offset, "alarmState: ", bacapp_EventState);
	fApplicationTags (tvb, tree, offset, "acknowledgedTransitions: ", bacapp_EventTransitionBits);

	fGetAlarmSummary (tvb, pinfo, tree, offset);
}
