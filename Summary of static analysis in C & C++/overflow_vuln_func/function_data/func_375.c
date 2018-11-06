void
proto_register_eap(void)
{
  static hf_register_info hf[] = {
	{ &hf_eap_code, { 
		"Code", "eap.code", FT_UINT8, BASE_DEC, 
		NULL, 0x0, "", HFILL }},
	{ &hf_eap_identifier, {
		"Id", "eap.id", FT_UINT8, BASE_DEC,
		NULL, 0x0, "", HFILL }},
	{ &hf_eap_len, {
		"Length", "eap.len", FT_UINT16, BASE_DEC,
		NULL, 0x0, "", HFILL }},
	{ &hf_eap_type, { 
		"Type", "eap.type", FT_UINT8, BASE_DEC, 
		NULL, 0x0, "", HFILL }},
  };
  static gint *ett[] = {
	&ett_eap,
  };

  proto_eap = proto_register_protocol("Extensible Authentication Protocol", 
				      "EAP", "eap");
  proto_register_field_array(proto_eap, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));
}
