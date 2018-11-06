static void dissect_r3_upstreammfgfield_lockstate (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item *ls_item = proto_tree_add_text (tree, tvb, start_offset, tvb_length_remaining (tvb, start_offset), "Lock State (0x%06x)", tvb_get_letoh24 (tvb, start_offset));
  proto_tree *ls_tree = proto_item_add_subtree (ls_item, ett_r3lockstate);
  guint ls = tvb_get_letoh24 (tvb, start_offset);

  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_passage, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_panic, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_lockout, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_relock, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_autoopen, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_nextauto, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_lockstate, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_wantstate, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_remote, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_update, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_exceptionspresent, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_exceptionsactive, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_timezonespresent, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_timezonesactive, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_autounlockspresent, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_autounlocksactive, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_uapmspresent, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_uapmsactive, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_uapmrelockspresent, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_uapmreslocksactive, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_nvramprotect, tvb, start_offset, 3, ls);
  proto_tree_add_boolean (ls_tree, hf_r3_lockstate_nvramchecksum, tvb, start_offset, 3, ls);
}
