void
proto_register_pppmux(void)
{
  static hf_register_info hf[] =
  {
    { &hf_pppmux_protocol,
      { "Protocol", "ppp.protocol", FT_UINT16, BASE_HEX,
        VALS(ppp_vals), 0x0, 
        "The protocol of the sub-frame.", HFILL }},
  };
	
  static gint *ett[] = {
    &ett_pppmux,
    &ett_pppmux_subframe,
    &ett_pppmux_subframe_hdr,
    &ett_pppmux_subframe_flags,
    &ett_pppmux_subframe_info,
  };

  proto_pppmux = proto_register_protocol("PPP Multiplexing",
				       "PPP PPPMux",
				      "pppmux");
  proto_register_field_array(proto_pppmux, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));
}
