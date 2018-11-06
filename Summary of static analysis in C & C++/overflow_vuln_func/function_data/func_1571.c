static void dissect_r3_upstreamcommand_mfg (tvbuff_t *tvb, guint32 start_offset _U_, guint32 length, packet_info *pinfo, proto_tree *tree)
{
  proto_item *mfg_item = NULL;
  proto_tree *mfg_tree = NULL;
  tvbuff_t *mfg_tvb = NULL;
  const gchar *cn;

  mfg_tvb = tvb_new_subset (tvb, 2, tvb_reported_length (tvb) - 2, tvb_reported_length (tvb) - 2);

  cn = val_to_str_ext_const (tvb_get_guint8 (tvb, 1), &r3_mfgfieldnames_ext, "[Unknown Mfg Field]");

  proto_tree_add_item (tree, hf_r3_mfgfield_length, tvb, 0, 1, ENC_LITTLE_ENDIAN);

  mfg_item = proto_tree_add_text (tree, tvb, 1, -1, "Upstream Manufacturing Field: %s (%u)", cn, tvb_get_guint8 (tvb, 1));
  mfg_tree = proto_item_add_subtree (mfg_item, ett_r3commandmfg);

  proto_tree_add_item (mfg_tree, hf_r3_mfgfield, tvb, 1, 1, ENC_LITTLE_ENDIAN);

  if (tvb_get_guint8 (tvb, 1) >= MFGFIELD_LAST)
    expert_add_info_format (pinfo, tree, PI_UNDECODED, PI_WARN, "Unknown manufacturing command value");
  else if (r3upstreammfgfield_dissect [tvb_get_guint8 (tvb, 1)])
    (*r3upstreammfgfield_dissect [tvb_get_guint8 (tvb, 1)]) (mfg_tvb, 0, length, pinfo, mfg_tree);
}
