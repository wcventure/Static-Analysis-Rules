static proto_item *
_proto_tree_add_bits_format_value(proto_tree *tree, const int hf_index,
				 tvbuff_t *tvb, const gint bit_offset,
				 const gint no_of_bits, void *value_ptr,
				 gchar *value_str)
{
	gint     offset;
	guint    length;
	guint8   tot_no_bits;
	char    *str;
	guint64  value = 0;
	header_field_info *hf_field;

	/
	TRY_TO_FAKE_THIS_ITEM(tree, hf_index, hf_field);

	if (hf_field->bitmask != 0) {
		REPORT_DISSECTOR_BUG(ep_strdup_printf(
					     "Incompatible use of proto_tree_add_bits_format_value"
					     " with field '%s' (%s) with bitmask != 0",
					     hf_field->abbrev, hf_field->name));
	}

	DISSECTOR_ASSERT(bit_offset >= 0);
	DISSECTOR_ASSERT(no_of_bits > 0);

	/
	offset = bit_offset>>3;

	/
	tot_no_bits = ((bit_offset&0x7) + no_of_bits);
	length      = tot_no_bits>>3;
	/
	if (tot_no_bits & 0x07)
		length++;

	if (no_of_bits < 65) {
		value = tvb_get_bits64(tvb, bit_offset, no_of_bits, ENC_BIG_ENDIAN);
	} else {
		DISSECTOR_ASSERT_NOT_REACHED();
		return NULL;
	}

	str = decode_bits_in_field(bit_offset, no_of_bits, value);

	strcat(str, " = ");
	strcat(str, hf_field->name);

	/
	switch (hf_field->type) {
	case FT_BOOLEAN:
		return proto_tree_add_boolean_format(tree, hf_index, tvb, offset, length, *(guint32 *)value_ptr,
						     "%s: %s", str, value_str);
		break;

	case FT_UINT8:
	case FT_UINT16:
	case FT_UINT24:
	case FT_UINT32:
		return proto_tree_add_uint_format(tree, hf_index, tvb, offset, length, *(guint32 *)value_ptr,
						  "%s: %s", str, value_str);
		break;

	case FT_UINT64:
		return proto_tree_add_uint64_format(tree, hf_index, tvb, offset, length, *(guint64 *)value_ptr,
						    "%s: %s", str, value_str);
		break;

	case FT_INT8:
	case FT_INT16:
	case FT_INT24:
	case FT_INT32:
		return proto_tree_add_int_format(tree, hf_index, tvb, offset, length, *(gint32 *)value_ptr,
						 "%s: %s", str, value_str);
		break;

	case FT_INT64:
		return proto_tree_add_int64_format(tree, hf_index, tvb, offset, length, *(gint64 *)value_ptr,
						   "%s: %s", str, value_str);
		break;

	case FT_FLOAT:
		return proto_tree_add_float_format(tree, hf_index, tvb, offset, length, *(float *)value_ptr,
						   "%s: %s", str, value_str);
		break;

	default:
		DISSECTOR_ASSERT_NOT_REACHED();
		return NULL;
		break;
	}
}
