void
proto_reg_handoff_skinny(void)
{
  static gboolean    skinny_prefs_initialized = FALSE;
  dissector_handle_t skinny_handle;

  if (!skinny_prefs_initialized) {
    /
    media_type_dissector_table = find_dissector_table("media_type");
    skinny_handle = new_create_dissector_handle(dissect_skinny, proto_skinny);
    dissector_add_uint("tcp.port", TCP_PORT_SKINNY, skinny_handle);
    ssl_dissector_add(SSL_PORT_SKINNY, "skinny", TRUE);
    skinny_prefs_initialized = TRUE;
  }
}
