int
dissect_gsm_map_USSD_String(gboolean implicit_tag _U_, tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree, int hf_index _U_) {
#line 425 "gsmmap.cnf"

  guint8		octet;
  guint8		length;
  guint8		out_len;
  static char bigbuf[1024];

  octet = tvb_get_guint8(tvb,0);
  length = tvb_get_guint8(tvb,1);

  out_len = gsm_sms_char_7bit_unpack(0, length, sizeof(bigbuf),
              tvb_get_ptr(tvb, offset+2, length), bigbuf);
  bigbuf[out_len] = '\0';
  gsm_sms_char_ascii_decode(bigbuf, bigbuf, out_len);
  bigbuf[1024] = '\0';
  
  proto_tree_add_text(tree, tvb, offset+2, length, "USSD String: %s", bigbuf);



  return offset;
}
