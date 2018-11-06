static void
pr_loc_response(tvbuff_t *tvb, proto_tree *tree, guint len, guint32 offset)
{
	guint32      bit_offset, spare_bits;
	guint32      value;
	float        fl_value;
	guint32      saved_offset;
	guint64      fix_type, velocity_incl, clock_incl, height_incl;
	const gchar *str = NULL;

	SHORT_DATA_CHECK(len, 11);
	saved_offset = offset;
	bit_offset   = offset << 3;

	/
	value = tvb_get_bits16(tvb, bit_offset, 14, ENC_BIG_ENDIAN);
	proto_tree_add_uint_bits_format_value(tree, hf_ansi_801_time_ref_cdma, tvb, bit_offset, 14, value * 50,
					      "%u frames (0x%04x)", value * 50, value);
	bit_offset += 14;

	/
	value = tvb_get_bits32(tvb, bit_offset, 25, ENC_BIG_ENDIAN);
	fl_value = (float)(-90.0 + ((float)value * 180 / 33554432));
	proto_tree_add_float_bits_format_value(tree, hf_ansi_801_lat, tvb, bit_offset, 25, fl_value,
					       "%.5f degrees %s (0x%08x)", fabs(fl_value), fl_value < 0 ? "South" : "North", value);
	bit_offset += 25;

	/
	value    = tvb_get_bits32(tvb, bit_offset, 26, ENC_BIG_ENDIAN);
	fl_value = (float)(-180.0 + ((float)value * 180 / 33554432));
	proto_tree_add_float_bits_format_value(tree, hf_ansi_801_long, tvb, bit_offset, 26, fl_value,
					       "%.5f degrees %s (0x%08x)", fabs(fl_value), fl_value < 0 ? "West" : "East", value);
	bit_offset += 26;

	/
	value    = tvb_get_bits8(tvb, bit_offset, 4);
	fl_value = (float)(5.625 * value);
	proto_tree_add_float_bits_format_value(tree, hf_ansi_801_loc_uncrtnty_ang, tvb, bit_offset, 4, fl_value,
					       "%.5f degrees (0x%02x)", fl_value, value);
	bit_offset += 4;

	/
	value = tvb_get_bits8(tvb, bit_offset, 5);
	switch (value)
	{
	case 0x1e: str = "> 12288.00 meters"; break;
	case 0x1f: str = "Not computable"; break;
	default:
		fl_value = (float)(0.5 * (1 << (value >> 1)));
		if (value & 0x01)
			fl_value *= 1.5f;
		str = wmem_strdup_printf(wmem_packet_scope(), "%.2f meters", fl_value);
	}
	proto_tree_add_uint_bits_format_value(tree, hf_ansi_801_loc_uncrtnty_a, tvb, bit_offset, 5, value,
					      "%s (0x%02x)", str, value);
	bit_offset += 5;

	/
	value = tvb_get_bits8(tvb, bit_offset, 5);
	switch (value)
	{
	case 0x1e: str = "> 12288.00 meters"; break;
	case 0x1f: str = "Not computable"; break;
	default:
		fl_value = (float)(0.5 * (1 << (value >> 1)));
		if (value & 0x01)
			fl_value *= 1.5f;
		str = wmem_strdup_printf(wmem_packet_scope(), "%.2f meters", fl_value);
	}
	proto_tree_add_uint_bits_format_value(tree, hf_ansi_801_loc_uncrtnty_p, tvb, bit_offset, 5, value,
					      "%s (0x%02x)", str, value);
	bit_offset += 5;

	/
	proto_tree_add_bits_ret_val(tree, hf_ansi_801_fix_type, tvb, bit_offset++, 1, &fix_type, ENC_BIG_ENDIAN);

	/
	proto_tree_add_bits_ret_val(tree, hf_ansi_801_velocity_incl, tvb, bit_offset++, 1, &velocity_incl, ENC_BIG_ENDIAN);


	if(velocity_incl)
	{
		/
		value = tvb_get_bits16(tvb, bit_offset, 9, ENC_BIG_ENDIAN);
		fl_value = (float)(0.25 * value);
		proto_tree_add_float_bits_format_value(tree, hf_ansi_801_velocity_hor, tvb, bit_offset, 9, fl_value,
						       "%.2f m/s (0x%04x)", fl_value, value);
		bit_offset += 9;

		/
		value = tvb_get_bits16(tvb, bit_offset, 10, ENC_BIG_ENDIAN);
		fl_value = (float)value * 360 / 1024;
		proto_tree_add_float_bits_format_value(tree, hf_ansi_801_heading, tvb, bit_offset, 10, fl_value,
						       "%.3f degrees (0x%04x)", fl_value, value);
		bit_offset += 10;

		if(fix_type)
		{
			/
			value = tvb_get_bits8(tvb, bit_offset, 8);
			fl_value = (float)(-64 + 0.5 * value);
			proto_tree_add_float_bits_format_value(tree, hf_ansi_801_velocity_ver, tvb, bit_offset, 8, fl_value,
							       "%.1f m/s (0x%02x)", fl_value, value);
			bit_offset += 8;
		}
	}

	/
	proto_tree_add_bits_ret_val(tree, hf_ansi_801_clock_incl, tvb, bit_offset++, 1, &clock_incl, ENC_BIG_ENDIAN);

	if(clock_incl)
	{
		/
		value = tvb_get_bits32(tvb, bit_offset, 18, ENC_BIG_ENDIAN);
		proto_tree_add_int_bits_format_value(tree, hf_ansi_801_clock_bias, tvb, bit_offset, 18, (gint32)value - 13000,
						     "%d ns (0x%06x)", (gint32)value - 13000, value);
		bit_offset += 18;

		/
		value = tvb_get_bits16(tvb, bit_offset, 16, ENC_BIG_ENDIAN);
		proto_tree_add_int_bits_format_value(tree, hf_ansi_801_clock_drift, tvb, bit_offset, 16, (gint16)value,
						     "%d ppb (ns/s) (0x%04x)", (gint16)value, value);
		bit_offset += 16;
	}

	/
	proto_tree_add_bits_ret_val(tree, hf_ansi_801_height_incl, tvb, bit_offset++, 1, &height_incl, ENC_BIG_ENDIAN);

	if(height_incl)
	{
		/
		value = tvb_get_bits16(tvb, bit_offset, 14, ENC_BIG_ENDIAN);
		proto_tree_add_int_bits_format_value(tree, hf_ansi_801_height, tvb, bit_offset, 14, (gint32)value - 500,
						     "%d m (0x%04x)", (gint32)value - 500, value);
		bit_offset += 14;

		/
		value = tvb_get_bits8(tvb, bit_offset, 5);
		switch (value)
		{
		case 0x1e: str = "> 12288.00 meters"; break;
		case 0x1f: str = "Not computable"; break;
		default:
			fl_value = (float)(0.5 * (1 << (value >> 1)));
			if (value & 0x01)
				fl_value *= 1.5f;
			str = wmem_strdup_printf(wmem_packet_scope(), "%.2f meters", fl_value);
		}
		proto_tree_add_uint_bits_format_value(tree, hf_ansi_801_loc_uncrtnty_v, tvb, bit_offset, 5, value,
						      "%s (0x%02x)", str, value);
		bit_offset += 5;
	}

	if(bit_offset & 0x07)
	{
		spare_bits = 8 - (bit_offset & 0x07);
		proto_tree_add_bits_item(tree, hf_ansi_801_reserved_bits, tvb, bit_offset, spare_bits, ENC_BIG_ENDIAN);
		bit_offset += spare_bits;
	}

	offset = bit_offset >> 3;

	EXTRANEOUS_DATA_CHECK(len, offset - saved_offset);
}
