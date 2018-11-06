static guint
get_skinny_pdu_len(packet_info *pinfo _U_, tvbuff_t *tvb, int offset)
{
  guint32 hdr_data_length;

  /
  hdr_data_length = tvb_get_letohl(tvb, offset);

  /
  return hdr_data_length + 8;
}
