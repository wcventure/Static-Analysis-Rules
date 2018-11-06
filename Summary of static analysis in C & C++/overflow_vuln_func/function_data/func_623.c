void
proto_reg_handoff_atalk(void)
{
  dissector_handle_t ddp_handle, nbp_handle, rtmp_request_handle;
  dissector_handle_t atp_handle;
  dissector_handle_t rtmp_data_handle, llap_handle;

  ddp_handle = create_dissector_handle(dissect_ddp, proto_ddp);
  dissector_add("ethertype", ETHERTYPE_ATALK, ddp_handle);
  dissector_add("chdlctype", ETHERTYPE_ATALK, ddp_handle);
  dissector_add("ppp.protocol", PPP_AT, ddp_handle);
  dissector_add("null.type", BSD_AF_APPLETALK, ddp_handle);

  nbp_handle = create_dissector_handle(dissect_nbp, proto_nbp);
  dissector_add("ddp.type", DDP_NBP, nbp_handle);

  atp_handle = create_dissector_handle(dissect_atp, proto_atp);
  dissector_add("ddp.type", DDP_ATP, atp_handle);

  asp_handle = create_dissector_handle(dissect_asp, proto_asp);

  rtmp_request_handle = create_dissector_handle(dissect_rtmp_request, proto_rtmp);
  rtmp_data_handle = create_dissector_handle(dissect_rtmp_data, proto_rtmp);
  dissector_add("ddp.type", DDP_RTMPREQ, rtmp_request_handle);
  dissector_add("ddp.type", DDP_RTMPDATA, rtmp_data_handle);

  llap_handle = create_dissector_handle(dissect_llap, proto_llap);
  dissector_add("wtap_encap", WTAP_ENCAP_LOCALTALK, llap_handle);

  register_init_routine( atp_defragment_init);
  register_init_routine( &asp_reinit);

  afp_handle = find_dissector("afp");
  data_handle = find_dissector("data");
}
