void
proto_register_bicc(void)
{
/
	static hf_register_info hf[] = {
		{ &hf_bicc_cic,
			{ "Call identification Code (CIC)",           "bicc.cic",
			FT_UINT32, BASE_HEX, NULL, 0x0,
			  "", HFILL }},
	};

/
	static gint *ett[] = {
		&ett_bicc
	};
	proto_bicc = proto_register_protocol("Bearer Independent Call Control ",
	    "BICC", "bicc"); 
/
	proto_register_field_array(proto_bicc, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));

	register_init_routine(isup_apm_defragment_init);
}
