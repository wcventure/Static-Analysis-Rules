void
dissect_eap(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
  e_eap       eaph;
  guint       len;
  proto_tree *ti;
  proto_tree *volatile eap_tree;

  if (check_col(pinfo->cinfo, COL_PROTOCOL))
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "EAP");
  if (check_col(pinfo->cinfo, COL_INFO))
    col_clear(pinfo->cinfo, COL_INFO);

  tvb_memcpy(tvb, (guint8 *)&eaph, 0, sizeof(eaph));
  eaph.eap_len = ntohs(eaph.eap_len);

  len = eaph.eap_len;

  set_actual_length(tvb, len);

  eap_tree = NULL;

  if (tree) {
    ti = proto_tree_add_item(tree, proto_eap, tvb, 0, len, FALSE);
    eap_tree = proto_item_add_subtree(ti, ett_eap);

    proto_tree_add_text(eap_tree, tvb, 0, 0, "Code: %s (%u) ",
			val_to_str(eaph.eap_code, eap_code_vals, "Unknown"),
			eaph.eap_code);

    proto_tree_add_uint(eap_tree, hf_eap_identifier, tvb, 1, 1, eaph.eap_id);
    proto_tree_add_uint(eap_tree, hf_eap_len,    tvb, 2, 2, eaph.eap_len);

    if (len > 4) {
	guint8 eap_type = tvb_get_guint8(tvb, 4);
	proto_tree_add_text(eap_tree, tvb, 4, 1, "Type: %s (%u)", 
			    val_to_str(eap_type, eap_type_vals, "Unknown"),
			    eap_type);
    }
    if (len > 5)
      call_dissector(data_handle,tvb_new_subset(tvb, 5,-1,tvb_reported_length_remaining(tvb,5)), pinfo, tree);
  }
}
