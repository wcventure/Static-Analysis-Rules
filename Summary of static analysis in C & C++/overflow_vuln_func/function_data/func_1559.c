void
proto_reg_handoff_erf(void)
{
  dissector_handle_t erf_handle;

  erf_handle = find_dissector("erf");
  dissector_add_uint("wtap_encap", WTAP_ENCAP_ERF, erf_handle);

  /
  data_handle = find_dissector("data");

  /
  ipv4_handle   = find_dissector("ip");
  ipv6_handle   = find_dissector("ipv6");

  /
  infiniband_handle      = find_dissector("infiniband");
  infiniband_link_handle = find_dissector("infiniband_link");

  /
  chdlc_handle  = find_dissector("chdlc");
  ppp_handle    = find_dissector("ppp_hdlc");
  frelay_handle = find_dissector("fr");
  mtp2_handle   = find_dissector("mtp2");

  /
  atm_untruncated_handle = find_dissector("atm_untruncated");

  /
  ethwithfcs_handle    = find_dissector("eth_withfcs");
  ethwithoutfcs_handle = find_dissector("eth_withoutfcs");
  
  sdh_handle = find_dissector("sdh");
}
