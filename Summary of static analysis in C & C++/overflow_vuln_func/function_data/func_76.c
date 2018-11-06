void proto_reg_handoff_ua3g(void)
{
	dissector_handle_t handle_ua3g = find_dissector("ua3g");

	/
	register_dissector_table("ua3g.opcode", "ua3g.opcode",
		FT_UINT8, BASE_HEX);

	dissector_add_uint("ua3g.opcode", 0x15, handle_ua3g);
}
