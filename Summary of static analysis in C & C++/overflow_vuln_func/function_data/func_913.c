gint
dissect_epl_pres(proto_tree *epl_tree, tvbuff_t *tvb, packet_info *pinfo, guint8 epl_src, gint offset)
{
	guint16  len;
	guint8  pdoversion;
	guint8  flags;

	if (epl_src != EPL_MN_NODEID)   /
	{
		proto_tree_add_item(epl_tree, hf_epl_pres_stat_cs, tvb, offset, 1, ENC_LITTLE_ENDIAN);
	}
	else /
	{
		proto_tree_add_item(epl_tree, hf_epl_pres_stat_ms, tvb, offset, 1, ENC_LITTLE_ENDIAN);
	}
	offset += 1;

	flags = tvb_get_guint8(tvb, offset);
	proto_tree_add_boolean(epl_tree, hf_epl_pres_ms, tvb, offset, 1, flags);
	proto_tree_add_boolean(epl_tree, hf_epl_pres_en, tvb, offset, 1, flags);
	proto_tree_add_boolean(epl_tree, hf_epl_pres_rd, tvb, offset, 1, flags);
	offset += 1;

	proto_tree_add_item(epl_tree, hf_epl_pres_pr, tvb, offset, 1, ENC_LITTLE_ENDIAN);
	proto_tree_add_item(epl_tree, hf_epl_pres_rs, tvb, offset, 1, ENC_LITTLE_ENDIAN);
	offset += 1;

	pdoversion = tvb_get_guint8(tvb, offset);
	proto_tree_add_item(epl_tree, hf_epl_pres_pdov, tvb, offset, 1, ENC_NA);
	offset += 2;

	/
	len = tvb_get_letohs(tvb, offset);
	proto_tree_add_uint(epl_tree, hf_epl_pres_size, tvb, offset, 2, len);

	col_append_fstr(pinfo->cinfo, COL_INFO, "RD = %d   size = %d   ver = %d.%d",
			(EPL_PDO_RD_MASK & flags), len, hi_nibble(pdoversion), lo_nibble(pdoversion));

	offset += 2;
	offset += dissect_epl_payload ( epl_tree, tvb, pinfo, offset, len );

	return offset;
}
