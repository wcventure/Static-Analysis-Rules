void
proto_register_dsi(void)
{

  static hf_register_info hf[] = {
    { &hf_dsi_flags,
      { "Flags",            "dsi.flags",
	FT_UINT8, BASE_HEX, VALS(flag_vals), 0x0,
      	"Indicates request or reply.", HFILL }},

    { &hf_dsi_command,
      { "Command",          "dsi.command",
	FT_UINT8, BASE_DEC, VALS(func_vals), 0x0,
      	"Represents a DSI command.", HFILL }},

    { &hf_dsi_requestid,
      { "Request ID",       "dsi.requestid",
	FT_UINT16, BASE_DEC, NULL, 0x0,
      	"Keeps track of which request this is.  Replies must match a Request.  IDs must be generated in sequential order.", HFILL }},

    { &hf_dsi_offset,
      { "Data offset",      "dsi.data_offset",
	FT_INT32, BASE_DEC, NULL, 0x0,
      	"Data offset", HFILL }},

    { &hf_dsi_error,
      { "Error code",       "dsi.error_code",
	FT_INT32, BASE_DEC, VALS(asp_error_vals), 0x0,
      	"Error code", HFILL }},

    { &hf_dsi_length,
      { "Length",           "dsi.length",
	FT_UINT32, BASE_DEC, NULL, 0x0,
      	"Total length of the data that follows the DSI header.", HFILL }},

    { &hf_dsi_reserved,
      { "Reserved",         "dsi.reserved",
	FT_UINT32, BASE_HEX, NULL, 0x0,
      	"Reserved for future use.  Should be set to zero.", HFILL }},

    { &hf_dsi_server_name,
      { "Server name",         "dsi.server_name",
	FT_UINT_STRING, BASE_NONE, NULL, 0x0,
      	"Server name", HFILL }},

    { &hf_dsi_server_type,
      { "Server type",         "dsi.server_type",
	FT_UINT_STRING, BASE_NONE, NULL, 0x0,
      	"Server type", HFILL }},

    { &hf_dsi_server_vers,
      { "AFP version",         "dsi.server_vers",
	FT_UINT_STRING, BASE_NONE, NULL, 0x0,
      	"AFP version", HFILL }},

    { &hf_dsi_server_uams,
      { "UAM",         "dsi.server_uams",
	FT_UINT_STRING, BASE_NONE, NULL, 0x0,
      	"UAM", HFILL }},

    { &hf_dsi_server_icon,
      { "Icon bitmap",         "dsi.server_icon",
	FT_BYTES, BASE_HEX, NULL, 0x0,
      	"Server icon bitmap", HFILL }},

    { &hf_dsi_server_signature,
      { "Server signature",         "dsi.server_signature",
	FT_BYTES, BASE_HEX, NULL, 0x0,
      	"Server signature", HFILL }},

    { &hf_dsi_server_flag,
      { "Flag",         "dsi.server_flag",
	FT_UINT16, BASE_HEX, NULL, 0x0,
      	"Server capabilities flag", HFILL }},
    { &hf_dsi_server_flag_copyfile,
      { "Support copyfile",      "dsi.server_flag.copyfile",
		FT_BOOLEAN, 16, NULL, AFPSRVRINFO_COPY,
      	"Server support copyfile", HFILL }},
    { &hf_dsi_server_flag_passwd,
      { "Support change password",      "dsi.server_flag.passwd",
		FT_BOOLEAN, 16, NULL, AFPSRVRINFO_PASSWD,
      	"Server support change password", HFILL }},
    { &hf_dsi_server_flag_no_save_passwd,
      { "Don't allow save password",      "dsi.server_flag.no_save_passwd",
		FT_BOOLEAN, 16, NULL, AFPSRVRINFO_NOSAVEPASSWD,
      	"Don't allow save password", HFILL }},
    { &hf_dsi_server_flag_srv_msg,
      { "Support server message",      "dsi.server_flag.srv_msg",
		FT_BOOLEAN, 16, NULL, AFPSRVRINFO_SRVMSGS,
      	"Support server message", HFILL }},
    { &hf_dsi_server_flag_srv_sig,
      { "Support server signature",      "dsi.server_flag.srv_sig",
		FT_BOOLEAN, 16, NULL, AFPSRVRINFO_SRVSIGNATURE,
      	"Support server signature", HFILL }},
    { &hf_dsi_server_flag_tcpip,
      { "Support TCP/IP",      "dsi.server_flag.tcpip",
		FT_BOOLEAN, 16, NULL, AFPSRVRINFO_TCPIP,
      	"Server support TCP/IP", HFILL }},
    { &hf_dsi_server_flag_notify,
      { "Support server notifications",      "dsi.server_flag.notify",
		FT_BOOLEAN, 16, NULL, AFPSRVRINFO_SRVNOTIFY,
      	"Server support notifications", HFILL }},
    { &hf_dsi_server_flag_fast_copy,
      { "Support fast copy",      "dsi.server_flag.fast_copy",
		FT_BOOLEAN, 16, NULL, AFPSRVRINFO_FASTBOZO,
      	"Server support fast copy", HFILL }},


    { &hf_dsi_server_addr_len,
      { "Length",          "dsi.server_addr.len",
	FT_UINT8, BASE_DEC, NULL, 0x0,
      	"Address length.", HFILL }},

    { &hf_dsi_server_addr_type,
      { "Type",          "dsi.server_addr.type",
	FT_UINT8, BASE_DEC, VALS(afp_server_addr_type_vals), 0x0,
      	"Address type.", HFILL }},

    { &hf_dsi_server_addr_value,
      { "Value",          "dsi.server_addr.value",
	FT_BYTES, BASE_HEX, NULL, 0x0,
      	"Address value", HFILL }},

  };
  static gint *ett[] = {
    &ett_dsi,
    &ett_dsi_status,
    &ett_dsi_status_server_flag,
    &ett_dsi_vers,
    &ett_dsi_uams,
    &ett_dsi_addr,
  };
  module_t *dsi_module;

  proto_dsi = proto_register_protocol("Data Stream Interface", "DSI", "dsi");
  proto_register_field_array(proto_dsi, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));

  dsi_module = prefs_register_protocol(proto_dsi, NULL);
  prefs_register_bool_preference(dsi_module, "desegment",
    "Desegment all DSI messages spanning multiple TCP segments",
    "Whether the DSI dissector should desegment all messages spanning multiple TCP segments",
    &dsi_desegment);
}
