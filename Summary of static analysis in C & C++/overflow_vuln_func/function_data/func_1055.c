static void dissect_authinfo(proto_item *pitem, tvbuff_t *tvb, int offset, int sumlen)
{
	proto_tree *authtree;
        e_pduhdr pduhdr;
	gchar *authstrval, *valstr;
	int len;

	authtree = proto_item_add_subtree(pitem, ett_iapp_auth);

	while (sumlen > 0)
	{
		tvb_memcpy(tvb, (guint8 *)&pduhdr, offset, sizeof(e_pduhdr));
		len = (((int)pduhdr.pdu_len_h) << 8) + pduhdr.pdu_len_l;

		authstrval = val_to_str(pduhdr.pdu_type, iapp_auth_type_vals,
			"Unknown PDU Type");
		valstr = authval_to_str(pduhdr.pdu_type, len, tvb, offset);
		proto_tree_add_text(authtree, tvb, offset, len + 3, "%s(%d) %s",
			authstrval, pduhdr.pdu_type, valstr);

		sumlen -= (len + 3);
		offset += (len + 3);
	}
}
