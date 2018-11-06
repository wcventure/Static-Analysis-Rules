void
proto_reg_handoff_eap(void)
{
  dissector_handle_t eap_handle;

  data_handle = find_dissector("data");
  eap_handle = create_dissector_handle(dissect_eap, proto_eap);
  dissector_add("ppp.protocol", PPP_EAP, eap_handle);
}
