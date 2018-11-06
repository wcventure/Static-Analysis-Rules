void proto_reg_handoff_noe(void)
{
	dissector_handle_t handle_noe = find_dissector("noe");

	/
	register_dissector_table("ua.opcode",
		"ua.opcode",
		FT_UINT8,
		BASE_HEX);

	dissector_add_uint("ua.opcode", 0x15, handle_noe);
}
