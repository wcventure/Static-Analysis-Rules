static void
dissect_ajp13(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
  /
  int offset = 0;
  guint32 value = 666;
  ajp13_conv_data *cd = (ajp13_conv_data*)0;

  /

  tcp_dissect_pdus(tvb, pinfo, tree,
                   TRUE,                   /
                   4,                      /
                   get_ajp13_pdu_len,      /
                   dissect_ajp13_tcp_pdu); /
}
