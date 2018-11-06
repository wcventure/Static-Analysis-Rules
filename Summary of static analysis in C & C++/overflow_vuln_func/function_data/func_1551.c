static int
dissect_skinny(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data)
{
  /
  /
  guint32 hdr_data_length;
  guint32 hdr_version;

  /

  if (tvb_captured_length(tvb) < 8)
  {
    return 0;
  }
  /
  hdr_data_length = tvb_get_letohl(tvb, 0);
  hdr_version     = tvb_get_letohl(tvb, 4);

  /

  if (
      (hdr_data_length < 4) ||
      ((hdr_version != BASIC_MSG_TYPE) &&
       (hdr_version != CM7_MSG_TYPE_A) &&
       (hdr_version != CM7_MSG_TYPE_B) &&
       (hdr_version != CM7_MSG_TYPE_UNKNOWN) &&
       (hdr_version != CM7_MSG_TYPE_C) &&
       (hdr_version != CM7_MSG_TYPE_D))
     )
  {
      /
      return 0;
  }

  /
  col_set_str(pinfo->cinfo, COL_PROTOCOL, "SKINNY");

  col_set_str(pinfo->cinfo, COL_INFO, "Skinny Client Control Protocol");

  tcp_dissect_pdus(tvb, pinfo, tree, skinny_desegment, 4,
                   get_skinny_pdu_len, dissect_skinny_pdu, data);

  return tvb_captured_length(tvb);
}
