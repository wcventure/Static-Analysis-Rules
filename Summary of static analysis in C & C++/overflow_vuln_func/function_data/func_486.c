void
proto_reg_handoff_chap(void)
{
  dissector_handle_t chap_handle = create_dissector_handle(dissect_chap, proto_chap);
  dissector_add("ppp.protocol", PPP_CHAP, chap_handle);

  /
  dissector_add("ethertype", PPP_CHAP, chap_handle);
  
  /
  dissector_add("sm_pco.protocol", PPP_CHAP, chap_handle);
}
