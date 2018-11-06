static void
proto_mpeg_descriptor_dissect_cable_delivery(tvbuff_t *tvb, guint offset, proto_tree *tree) {

	guint32 frequency, symbol_rate;

	frequency = tvb_get_ntohl(tvb, offset);
	proto_tree_add_string_format_value(tree, hf_mpeg_descr_cable_delivery_frequency, tvb, offset, 4,
		"Frequency", "%2u%02u,%02u%02u MHz",
		MPEG_SECT_BCD44_TO_DEC(frequency >> 24),
		MPEG_SECT_BCD44_TO_DEC(frequency >> 16),
		MPEG_SECT_BCD44_TO_DEC(frequency >> 8),
		MPEG_SECT_BCD44_TO_DEC(frequency));
	offset += 4;

	proto_tree_add_item(tree, hf_mpeg_descr_cable_delivery_reserved, tvb, offset, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_cable_delivery_fec_outer, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	proto_tree_add_item(tree, hf_mpeg_descr_cable_delivery_modulation, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	symbol_rate = tvb_get_ntohl(tvb, offset) >> 4;
	proto_tree_add_string_format_value(tree, hf_mpeg_descr_cable_delivery_symbol_rate, tvb, offset, 4,
		"Symbol Rate", "%2u%02u%02u%01u,%01u KSymbol/s",
		MPEG_SECT_BCD44_TO_DEC(symbol_rate >> 24),
		MPEG_SECT_BCD44_TO_DEC(symbol_rate >> 16),
		MPEG_SECT_BCD44_TO_DEC(symbol_rate >> 8),
		MPEG_SECT_BCD44_TO_DEC(symbol_rate) / 10,
		MPEG_SECT_BCD44_TO_DEC(symbol_rate) % 10);

	offset += 3;
	proto_tree_add_item(tree, hf_mpeg_descr_cable_delivery_fec_inner, tvb, offset, 1, ENC_BIG_ENDIAN);


}
