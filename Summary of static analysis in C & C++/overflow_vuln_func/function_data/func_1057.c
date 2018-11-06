static void
dissect_pdus(tvbuff_t *tvb, int offset, proto_tree *pdutree, int pdulen)
{
	e_pduhdr pduhdr;
	int len;
	gchar *pdustrval, *valstr;
	proto_item *ti;

	if (!pdulen)
	{
		proto_tree_add_text(pdutree, tvb, offset, 0, "No PDUs found");
		return;
	}

	while (pdulen > 0)
	{
		tvb_memcpy(tvb, (guint8 *)&pduhdr, offset, sizeof(e_pduhdr));
		len = (((int)pduhdr.pdu_len_h) << 8) + pduhdr.pdu_len_l;

		pdustrval = val_to_str(pduhdr.pdu_type, iapp_pdu_type_vals,
			"Unknown PDU Type");
		valstr = pduval_to_str(pduhdr.pdu_type, len, tvb, offset);
		ti = proto_tree_add_text(pdutree, tvb, offset, len + 3, "%s(%d) %s",
			pdustrval, pduhdr.pdu_type, valstr);

		if (pduhdr.pdu_type == IAPP_PDU_CAPABILITY)
			dissect_caps(ti, tvb, offset);

		if (pduhdr.pdu_type == IAPP_PDU_AUTHINFO)
			dissect_authinfo(ti, tvb, offset + 3, len);

		pdulen -= (len + 3);
		offset += (len + 3);
	}
}
