static void
dissect_rsn_ie(proto_tree * tree, tvbuff_t * tvb, int offset,
	       guint32 tag_len, const guint8 *tag_val)
{
  guint32 tag_val_off = 0;
  guint16 rsn_capab;
  char out_buff[SHORT_STR];
  int i, j, count;
  proto_item *cap_item;
  proto_tree *cap_tree;

  if (tag_val_off + 2 > tag_len) {
    proto_tree_add_string(tree, tag_interpretation, tvb, offset, tag_len,
			  "Not interpreted");
    return;
  }

  g_snprintf(out_buff, SHORT_STR, "RSN IE, version %u",
	   pletohs(&tag_val[tag_val_off]));
  proto_tree_add_string(tree, tag_interpretation, tvb, offset, 2, out_buff);

  offset += 2;
  tag_val_off += 2;

  if (tag_val_off + 4 > tag_len)
    goto done;

  /
  if (!memcmp(&tag_val[tag_val_off], RSN_OUI, 3)) {
    g_snprintf(out_buff, SHORT_STR, "Multicast cipher suite: %s",
	     wpa_cipher_idx2str(tag_val[tag_val_off + 3]));
    proto_tree_add_string(tree, tag_interpretation, tvb, offset, 4, out_buff);
    offset += 4;
    tag_val_off += 4;
  }

  if (tag_val_off + 2 > tag_len)
    goto done;

  /
  count = pletohs(tag_val + tag_val_off);
  g_snprintf(out_buff, SHORT_STR, "# of unicast cipher suites: %u", count);
  proto_tree_add_string(tree, tag_interpretation, tvb, offset, 2, out_buff);
  offset += 2;
  tag_val_off += 2;
  i = 1;
  while (tag_val_off + 4 <= tag_len && i <= count) {
    if (memcmp(&tag_val[tag_val_off], RSN_OUI, 3) != 0)
      goto done;
    g_snprintf(out_buff, SHORT_STR, "Unicast cipher suite %u: %s",
	     i, wpa_cipher_idx2str(tag_val[tag_val_off + 3]));
    proto_tree_add_string(tree, tag_interpretation, tvb, offset, 4, out_buff);
    offset += 4;
    tag_val_off += 4;
    i++;
  }

  if (i <= count || tag_val_off + 2 > tag_len)
    goto done;

  /
  count = pletohs(tag_val + tag_val_off);
  g_snprintf(out_buff, SHORT_STR, "# of auth key management suites: %u", count);
  proto_tree_add_string(tree, tag_interpretation, tvb, offset, 2, out_buff);
  offset += 2;
  tag_val_off += 2;
  i = 1;
  while (tag_val_off + 4 <= tag_len && i <= count) {
    if (memcmp(&tag_val[tag_val_off], RSN_OUI, 3) != 0)
      goto done;
    g_snprintf(out_buff, SHORT_STR, "auth key management suite %u: %s",
	     i, wpa_keymgmt_idx2str(tag_val[tag_val_off + 3]));
    proto_tree_add_string(tree, tag_interpretation, tvb, offset, 4, out_buff);
    offset += 4;
    tag_val_off += 4;
    i++;
  }

  if (i <= count || tag_val_off + 2 > tag_len)
    goto done;

  rsn_capab = pletohs(&tag_val[tag_val_off]);
  g_snprintf(out_buff, SHORT_STR, "RSN Capabilities 0x%04x", rsn_capab);
  cap_item = proto_tree_add_uint_format(tree, rsn_cap, tvb,
					offset, 2, rsn_capab,
					"RSN Capabilities: 0x%04X", rsn_capab);
  cap_tree = proto_item_add_subtree(cap_item, ett_rsn_cap_tree);
  proto_tree_add_boolean(cap_tree, rsn_cap_preauth, tvb, offset, 2,
			 rsn_capab);
  proto_tree_add_boolean(cap_tree, rsn_cap_no_pairwise, tvb, offset, 2,
			 rsn_capab);
  proto_tree_add_uint(cap_tree, rsn_cap_ptksa_replay_counter, tvb, offset, 2,
		      rsn_capab);
  proto_tree_add_uint(cap_tree, rsn_cap_gtksa_replay_counter, tvb, offset, 2,
		      rsn_capab);
  offset += 2;
  tag_val_off += 2;

  if (tag_val_off + 2 > tag_len)
    goto done;

  count = pletohs(tag_val + tag_val_off);
  g_snprintf(out_buff, SHORT_STR, "# of PMKIDs: %u", count);
  proto_tree_add_string(tree, tag_interpretation, tvb, offset, 2, out_buff);
  offset += 2;
  tag_val_off += 2;

  /
  for (i = 0; i < count; i++) {
    char *pos;
    if (tag_val_off + PMKID_LEN > tag_len)
      goto done;
    pos = out_buff;
    pos += g_snprintf(pos, out_buff + SHORT_STR - pos, "PMKID %u: ", i);
    for (j = 0; j < PMKID_LEN; j++) {
      pos += g_snprintf(pos, out_buff + SHORT_STR - pos, "%02X",
		      tag_val[tag_val_off + j]);
    }
    proto_tree_add_string(tree, tag_interpretation, tvb, offset,
			  PMKID_LEN, out_buff);
    offset += PMKID_LEN;
    tag_val_off += PMKID_LEN;
  }

 done:
  if (tag_val_off < tag_len)
    proto_tree_add_string(tree, tag_interpretation, tvb, offset,
			  tag_len - tag_val_off, "Not interpreted");
}
