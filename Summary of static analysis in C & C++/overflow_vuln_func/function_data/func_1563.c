static void dissect_serialnumber (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree, int hf_index)
{
  proto_item *sn_item = NULL;
  proto_tree *sn_tree = NULL;
  const gchar *s = NULL;

  tvb_ensure_bytes_exist (tvb, start_offset, 16);

  sn_item = proto_tree_add_item (tree, hf_index, tvb, start_offset, 16, ENC_ASCII|ENC_NA);
  sn_tree = proto_item_add_subtree (sn_item, ett_r3serialnumber);

  s = tvb_get_ephemeral_string (tvb, start_offset +  0, 2);
  proto_tree_add_text (sn_tree, tvb, start_offset +  0, 2, "Manufacturer .. : %s (%s)", s, str_to_str (s, r3_snmanufacturernames, "[Unknown]"));
  s = tvb_get_ephemeral_string (tvb, start_offset +  2, 1);
  proto_tree_add_text (sn_tree, tvb, start_offset +  2, 1, "Year .......... : %s (%s)", s, str_to_str (s, r3_snyearnames, "[Unknown]"));
  s = tvb_get_ephemeral_string (tvb, start_offset +  3, 2);
  proto_tree_add_text (sn_tree, tvb, start_offset +  3, 2, "Week .......... : %s",      s);
  s = tvb_get_ephemeral_string (tvb, start_offset +  5, 1);
  proto_tree_add_text (sn_tree, tvb, start_offset +  5, 1, "Model ......... : %s (%s)", s, str_to_str (s, r3_snmodelnames, "[Unknown]"));
  s = tvb_get_ephemeral_string (tvb, start_offset +  6, 4);
  proto_tree_add_text (sn_tree, tvb, start_offset +  6, 4, "Sequence ...... : %s",      s);
  s = tvb_get_ephemeral_string (tvb, start_offset + 10, 1);
  proto_tree_add_text (sn_tree, tvb, start_offset + 10, 1, "Group ......... : %s (%s)", s, str_to_str (s, r3_sngroupnames, "[Unknown]"));
  s = tvb_get_ephemeral_string (tvb, start_offset + 11, 1);
  proto_tree_add_text (sn_tree, tvb, start_offset + 11, 1, "NID ........... : %s (%s)", s, str_to_str (s, r3_snnidnames, "[Unknown]"));
  s = tvb_get_ephemeral_string (tvb, start_offset + 12, 2);
  proto_tree_add_text (sn_tree, tvb, start_offset + 12, 2, "HID ........... : %s (%s)", s, str_to_str (s, r3_snhidnames, "[Unknown]"));
  s = tvb_get_ephemeral_string (tvb, start_offset + 14, 1);
  proto_tree_add_text (sn_tree, tvb, start_offset + 14, 1, "Power Supply .. : %s (%s)", s, str_to_str (s, r3_snpowersupplynames, "[Unknown]"));
  s = tvb_get_ephemeral_string (tvb, start_offset + 15, 1);
  proto_tree_add_text (sn_tree, tvb, start_offset + 15, 1, "Mortise ....... : %s (%s)", s, str_to_str (s, r3_snmortisenames, "[Unknown]"));
}
