static void
proto_mpeg_descriptor_dissect_satellite_delivery(tvbuff_t *tvb, guint offset, proto_tree *tree)
{

	guint32 frequency, symbol_rate;
	guint16 orbital_position;
	guint8  modulation_system;

	frequency = tvb_get_ntohl(tvb, offset);
	proto_tree_add_string_format_value(tree, hf_mpeg_descr_satellite_delivery_frequency, tvb, offset, 4,
		"Frequency", "%2u%01u,%01u%02u%02u GHz",
		MPEG_SECT_BCD44_TO_DEC(frequency >> 24),
		MPEG_SECT_BCD44_TO_DEC(frequency >> 16) / 10,
		MPEG_SECT_BCD44_TO_DEC(frequency >> 16) % 10,
		MPEG_SECT_BCD44_TO_DEC(frequency >> 8),
		MPEG_SECT_BCD44_TO_DEC(frequency));
	offset += 4;

	orbital_position = tvb_get_ntohs(tvb, offset);
	proto_tree_add_string_format_value(tree, hf_mpeg_descr_satellite_delivery_orbital_position, tvb, offset, 2,
		"Orbital Position", "%2u%1u,%1u degrees",
		MPEG_SECT_BCD44_TO_DEC(orbital_position >> 8),
		MPEG_SECT_BCD44_TO_DEC(orbital_position) / 10,
		MPEG_SECT_BCD44_TO_DEC(orbital_position) % 10);
	offset += 2;

	modulation_system = tvb_get_guint8(tvb, offset) & MPEG_DESCR_SATELLITE_DELIVERY_MODULATION_SYSTEM_MASK;

	proto_tree_add_item(tree, hf_mpeg_descr_satellite_delivery_west_east_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_satellite_delivery_polarization, tvb, offset, 1, ENC_BIG_ENDIAN);
	if (modulation_system)
		proto_tree_add_item(tree, hf_mpeg_descr_satellite_delivery_roll_off, tvb, offset, 1, ENC_BIG_ENDIAN);
	else
		proto_tree_add_item(tree, hf_mpeg_descr_satellite_delivery_zero, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_satellite_delivery_modulation_system, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_satellite_delivery_modulation_type, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	symbol_rate = tvb_get_ntohl(tvb, offset) >> 4;
	proto_tree_add_string_format_value(tree, hf_mpeg_descr_satellite_delivery_symbol_rate, tvb, offset, 4,
		"Symbol Rate", "%2u%02u%02u%01u,%01u KSymbol/s",
		MPEG_SECT_BCD44_TO_DEC(symbol_rate >> 24),
		MPEG_SECT_BCD44_TO_DEC(symbol_rate >> 16),
		MPEG_SECT_BCD44_TO_DEC(symbol_rate >> 8),
		MPEG_SECT_BCD44_TO_DEC(symbol_rate) / 10,
		MPEG_SECT_BCD44_TO_DEC(symbol_rate) % 10);

	offset += 3;
	proto_tree_add_item(tree, hf_mpeg_descr_satellite_delivery_fec_inner, tvb, offset, 1, ENC_BIG_ENDIAN);

}
