static int
dissect_camel_camelPDU(gboolean implicit_tag _U_, tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree, int hf_index) {

  char *version_ptr, *version_str;

  opcode = 0;
  application_context_version = 0;
  if (pinfo->private_data != NULL){
    version_ptr = strrchr(pinfo->private_data,'.');
    if (version_ptr) {
      application_context_version = atoi(version_ptr+1);
    }
  }

  camel_pdu_type = tvb_get_guint8(tvb, offset)&0x0f;
  /
  camel_pdu_size = tvb_get_guint8(tvb, offset+1)+2;

  if (check_col(pinfo->cinfo, COL_INFO)){
    /
    col_append_fstr(pinfo->cinfo, COL_INFO, val_to_str(camel_pdu_type, camelPDU_vals, "Unknown Camel (%u)"));
  }

  offset = dissect_ber_choice(pinfo, tree, tvb, offset,
                              camelPDU_choice, hf_index, ett_camel_camelPDU, NULL);

  return offset;
}
