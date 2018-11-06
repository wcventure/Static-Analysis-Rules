static void
dissect_atm_cell(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
    proto_tree *atm_tree)
{
  int          offset;
  proto_tree   *aal_tree;
  proto_item   *ti;
  guint8       octet;
  int          err;
  guint8       vpi;
  guint16      vci;
  guint8       pt;
  guint8       aal;
  guint16      aal3_4_hdr, aal3_4_trlr;
  guint16      oam_crc;
  gint         length;
  guint16      crc10;
  tvbuff_t     *next_tvb;

  octet = tvb_get_guint8(tvb, 0);
  proto_tree_add_text(atm_tree, tvb, 0, 1, "GFC: 0x%x", octet >> 4);
  vpi = (octet & 0xF0) << 4;
  octet = tvb_get_guint8(tvb, 1);
  vpi |= octet >> 4;
  proto_tree_add_uint(atm_tree, hf_atm_vpi, tvb, 0, 2, vpi);
  vci = (octet & 0x0F) << 12;
  octet = tvb_get_guint8(tvb, 2);
  vci |= octet << 4;
  octet = tvb_get_guint8(tvb, 3);
  vci |= octet >> 4;
  proto_tree_add_uint(atm_tree, hf_atm_vci, tvb, 1, 3, vci);
  pt = (octet >> 1) & 0x7;
  proto_tree_add_text(atm_tree, tvb, 3, 1, "Payload Type: %s",
        val_to_str(pt, pt_vals, "Unknown (%u)"));
  proto_tree_add_text(atm_tree, tvb, 3, 1, "Cell Loss Priority: %s",
	(octet & 0x01) ? "Low priority" : "High priority");
  ti = proto_tree_add_text(atm_tree, tvb, 4, 1, "Header Error Check: 0x%02x",
	  tvb_get_guint8(tvb, 4));
  err = get_header_err(tvb_get_ptr(tvb, 0, 5));
  if (err == NO_ERROR_DETECTED)
    proto_item_append_text(ti, " (correct)");
  else if (err == UNCORRECTIBLE_ERROR)
    proto_item_append_text(ti, " (uncorrectable error)");
  else
    proto_item_append_text(ti, " (error in bit %d)", err);
  offset = 5;

  /
  aal = pinfo->pseudo_header->atm.aal;
  if (aal == AAL_USER) {
	/
	if (((vci == 3 || vci == 4) && ((pt & 0x5) == 0)) ||
	    ((pt & 0x6) == 0x4))
		aal = AAL_OAMCELL;
  }

  switch (aal) {

  case AAL_1:
    if (check_col(pinfo->cinfo, COL_PROTOCOL))
      col_set_str(pinfo->cinfo, COL_PROTOCOL, "AAL1");
    if (check_col(pinfo->cinfo, COL_INFO))
      col_clear(pinfo->cinfo, COL_INFO);
    ti = proto_tree_add_item(tree, proto_aal1, tvb, offset, -1, FALSE);
    aal_tree = proto_item_add_subtree(ti, ett_aal1);
    octet = tvb_get_guint8(tvb, offset);
    proto_tree_add_text(aal_tree, tvb, offset, 1, "CSI: %u", octet >> 7);
    proto_tree_add_text(aal_tree, tvb, offset, 1, "Sequence Count: %u",
		(octet >> 4) & 0x7);
    if (check_col(pinfo->cinfo, COL_INFO)) {
      col_add_fstr(pinfo->cinfo, COL_INFO, "Sequence count = %u",
		(octet >> 4) & 0x7);
    }
    proto_tree_add_text(aal_tree, tvb, offset, 1, "CRC: 0x%x",
		(octet >> 1) & 0x7);
    proto_tree_add_text(aal_tree, tvb, offset, 1, "Parity: %u",
		octet & 0x1);
    offset++;

    proto_tree_add_text(aal_tree, tvb, offset, 47, "Payload");
    break;

  case AAL_3_4:
    /
    if (check_col(pinfo->cinfo, COL_PROTOCOL))
      col_set_str(pinfo->cinfo, COL_PROTOCOL, "AAL3/4");
    if (check_col(pinfo->cinfo, COL_INFO))
      col_clear(pinfo->cinfo, COL_INFO);
    ti = proto_tree_add_item(tree, proto_aal3_4, tvb, offset, -1, FALSE);
    aal_tree = proto_item_add_subtree(ti, ett_aal3_4);
    aal3_4_hdr = tvb_get_ntohs(tvb, offset);
    if (check_col(pinfo->cinfo, COL_INFO)) {
      col_add_fstr(pinfo->cinfo, COL_INFO, "%s, sequence number = %u",
		val_to_str(aal3_4_hdr >> 14, st_vals, "Unknown (%u)"),
		(aal3_4_hdr >> 10) & 0xF);
    }
    proto_tree_add_text(aal_tree, tvb, offset, 2, "Segment Type: %s",
		val_to_str(aal3_4_hdr >> 14, st_vals, "Unknown (%u)"));
    proto_tree_add_text(aal_tree, tvb, offset, 2, "Sequence Number: %u",
		(aal3_4_hdr >> 10) & 0xF);
    proto_tree_add_text(aal_tree, tvb, offset, 2, "Multiplex ID: %u",
		aal3_4_hdr & 0x3FF);
    offset += 2;

    proto_tree_add_text(aal_tree, tvb, offset, 44, "Information");
    offset += 44;

    aal3_4_trlr = tvb_get_ntohs(tvb, offset);
    proto_tree_add_text(aal_tree, tvb, offset, 2, "Length Indicator: %u",
		(aal3_4_trlr >> 10) & 0x3F);
    length = tvb_length_remaining(tvb, 5);
    crc10 = update_crc10_by_bytes(0, tvb_get_ptr(tvb, 5, length),
	length);
    proto_tree_add_text(aal_tree, tvb, offset, 2, "CRC: 0x%03x (%s)",
		aal3_4_trlr & 0x3FF,
		(crc10 == 0) ? "correct" : "incorrect");
    break;

  case AAL_OAMCELL:
    if (check_col(pinfo->cinfo, COL_PROTOCOL))
      col_set_str(pinfo->cinfo, COL_PROTOCOL, "OAM AAL");
    if (check_col(pinfo->cinfo, COL_INFO))
      col_clear(pinfo->cinfo, COL_INFO);
    ti = proto_tree_add_item(tree, proto_oamaal, tvb, offset, -1, FALSE);
    aal_tree = proto_item_add_subtree(ti, ett_oamaal);
    octet = tvb_get_guint8(tvb, offset);
    if (check_col(pinfo->cinfo, COL_INFO))
      col_add_fstr(pinfo->cinfo, COL_INFO, "%s",
		val_to_str(octet >> 4, oam_type_vals, "Unknown (%u)"));
    proto_tree_add_text(aal_tree, tvb, offset, 1, "OAM Type: %s",
		val_to_str(octet >> 4, oam_type_vals, "Unknown (%u)"));
    switch (octet >> 4) {

    case OAM_TYPE_FM:
      proto_tree_add_text(aal_tree, tvb, offset, 1, "Function Type: %s",
		val_to_str(octet & 0x0F, ft_fm_vals, "Unknown (%u)"));
      break;

    case OAM_TYPE_PM:
      proto_tree_add_text(aal_tree, tvb, offset, 1, "Function Type: %s",
		val_to_str(octet & 0x0F, ft_pm_vals, "Unknown (%u)"));
      break;

    case OAM_TYPE_AD:
      proto_tree_add_text(aal_tree, tvb, offset, 1, "Function Type: %s",
		val_to_str(octet & 0x0F, ft_ad_vals, "Unknown (%u)"));
      break;

    default:
      proto_tree_add_text(aal_tree, tvb, offset, 1, "Function Type: %u",
		octet & 0x0F);
      break;
    }
    offset += 1;

    proto_tree_add_text(aal_tree, tvb, offset, 45, "Function-specific information");
    offset += 45;

    length = tvb_length_remaining(tvb, 5);
    crc10 = update_crc10_by_bytes(0, tvb_get_ptr(tvb, 5, length),
	length);
    oam_crc = tvb_get_ntohs(tvb, offset);
    proto_tree_add_text(aal_tree, tvb, offset, 2, "CRC-10: 0x%03x (%s)",
		oam_crc & 0x3FF,
		(crc10 == 0) ? "correct" : "incorrect");
    break;

  default:
    next_tvb = tvb_new_subset(tvb, offset, -1, -1);
    call_dissector(data_handle, next_tvb, pinfo, tree);
    break;
  }
}
