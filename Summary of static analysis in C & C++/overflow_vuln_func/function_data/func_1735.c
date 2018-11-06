void
proto_reg_handoff_isup(void)
{
  dissector_handle_t isup_handle;
  dissector_handle_t application_isup_handle;
 
  isup_handle = create_dissector_handle(dissect_isup, proto_isup);
  application_isup_handle = create_dissector_handle(dissect_application_isup, proto_isup);
  dissector_add("mtp3.service_indicator", MTP3_ISUP_SERVICE_INDICATOR, isup_handle);
  dissector_add_string("media_type","application/isup", application_isup_handle);
  dissector_add_string("tali.opcode", "isot", isup_handle);

}
