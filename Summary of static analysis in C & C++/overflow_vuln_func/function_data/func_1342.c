static void
dissect_map (tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{
  guint8 i, numie;
  guint16 pos;
  guint16 sid;
  guint8 iuc;
  guint16 offset;
  guint32 ie, temp, mask;
  proto_item *it;
  proto_tree *map_tree;
  guint8 upchid, ucd_count;


  numie = tvb_get_guint8 (tvb, 2);
  upchid = tvb_get_guint8 (tvb, 0);
  ucd_count = tvb_get_guint8 (tvb, 1);

  if (check_col (pinfo->cinfo, COL_INFO))
    {
      col_clear (pinfo->cinfo, COL_INFO);
      if (upchid > 0)
	col_add_fstr (pinfo->cinfo, COL_INFO,
		      "Map Message:  Channel ID = %u (U%u), UCD Count = %u,  # IE's = %u",
		      upchid, upchid - 1, ucd_count, numie);
      else
	col_add_fstr (pinfo->cinfo, COL_INFO,
		      "Map Message:  Channel ID = %u (Telephony Return), UCD Count = %u, # IE's = %u",
		      upchid, ucd_count, numie);
    }

  if (tree)
    {
      it =
	proto_tree_add_protocol_format (tree, proto_docsis_map, tvb, 0,
					tvb_length_remaining (tvb, 0),
					"MAP Message");
      map_tree = proto_item_add_subtree (it, ett_docsis_map);

      proto_tree_add_item (map_tree, hf_docsis_map_upstream_chid, tvb, 0, 1,
			   FALSE);
      proto_tree_add_item (map_tree, hf_docsis_map_ucd_count, tvb, 1, 1,
			   FALSE);
      proto_tree_add_item (map_tree, hf_docsis_map_numie, tvb, 2, 1, FALSE);
      proto_tree_add_item (map_tree, hf_docsis_map_rsvd, tvb, 3, 1, FALSE);
      proto_tree_add_item (map_tree, hf_docsis_map_alloc_start, tvb, 4, 4,
			   FALSE);
      proto_tree_add_item (map_tree, hf_docsis_map_ack_time, tvb, 8, 4,
			   FALSE);
      proto_tree_add_item (map_tree, hf_docsis_map_rng_start, tvb, 12, 1,
			   FALSE);
      proto_tree_add_item (map_tree, hf_docsis_map_rng_end, tvb, 13, 1,
			   FALSE);
      proto_tree_add_item (map_tree, hf_docsis_map_data_start, tvb, 14, 1,
			   FALSE);
      proto_tree_add_item (map_tree, hf_docsis_map_data_end, tvb, 15, 1,
			   FALSE);

      pos = 16;
      for (i = 0; i < numie; i++)
	{
	  ie = tvb_get_ntohl (tvb, pos);
	  mask = 0xFFFC0000;
	  temp = (ie & mask);
	  temp = temp >> 18;
	  sid = (guint16) (temp & 0x3FFF);
	  mask = 0x3C000;
	  temp = (ie & mask);
	  temp = temp >> 14;
	  iuc = (guint8) (temp & 0x0F);
	  mask = 0x3FFF;
	  offset = (guint16) (ie & mask);
	  proto_tree_add_item_hidden(map_tree, hf_docsis_map_sid, tvb, pos, 4, FALSE);
	  proto_tree_add_item_hidden(map_tree, hf_docsis_map_iuc, tvb, pos, 4, FALSE);
	  proto_tree_add_item_hidden(map_tree, hf_docsis_map_offset, tvb, pos, 4, FALSE);
	  if (sid == 0x3FFF)
	    proto_tree_add_uint_format (map_tree, hf_docsis_map_ie, tvb, pos, 4,
				      ie, "SID = 0x%x (All CM's), IUC = %s, Offset = %u",
				      sid, val_to_str (iuc, iuc_vals, "%s"),
				      offset);
	  else
	    proto_tree_add_uint_format (map_tree, hf_docsis_map_ie, tvb, pos, 4,
				      ie, "SID = %u, IUC = %s, Offset = %u",
				      sid, val_to_str (iuc, iuc_vals, "%s"),
				      offset);
	  pos = pos + 4;
	}			/
    }				/


}
