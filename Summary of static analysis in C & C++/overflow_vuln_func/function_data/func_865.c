static void
dissect_bacapp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	guint8 tmp, bacapp_type;

	tmp = tvb_get_guint8(tvb, 0);
	bacapp_type = (tmp >> 4) & 0x0f;

	if (check_col(pinfo->cinfo, COL_INFO))
		col_set_str(pinfo->cinfo, COL_INFO, val_to_str(bacapp_type, bacapp_type_name, "#### unknown APDU ##### "));

	/
	switch (bacapp_type) {
	case 0:	/
		fConfirmedServiceRequest(tvb, pinfo, tree);
		break;
	case 1:	/
		fUnconfirmedServiceRequest(tvb, pinfo, tree);
		break;
	case 2:	/
		fSimpleAcknowledge(tvb, pinfo, tree);
		break;
	case 3:	/
		fComplexAcknowledge(tvb, pinfo, tree);
		break;
	case 4:	/
		fSegmentedAcknowledge(tvb, tree);
		break;
	case 5:	/
		fError(tvb, tree);
		break;
	case 6:	/
		fReject(tvb, tree);
		break;
	case 7:	/
		fAbort(tvb, tree);
		break;
	}
}
