proto_item *
proto_tree_add_split_bits_item_ret_val(proto_tree *tree, const int hf_index, tvbuff_t *tvb,
				       const guint bit_offset, const crumb_spec_t *crumb_spec,
				       guint64 *return_value)
{
	proto_item *pi;
	gint        no_of_bits;
	gint        octet_offset;
	guint       mask_initial_bit_offset;
	guint       mask_greatest_bit_offset;
	guint       octet_length;
	guint8      i;
	char       *bf_str;
	char        lbl_str[ITEM_LABEL_LENGTH];
	guint64     value;
	guint64     composite_bitmask;
	guint64     composite_bitmap;

	header_field_info       *hf_field;
	const true_false_string *tfstring;

	/
	PROTO_REGISTRAR_GET_NTH(hf_index, hf_field);

	if (hf_field->bitmask != 0) {
		REPORT_DISSECTOR_BUG(ep_strdup_printf(
					     "Incompatible use of proto_tree_add_split_bits_item_ret_val"
					     " with field '%s' (%s) with bitmask != 0",
					     hf_field->abbrev, hf_field->name));
	}

	mask_initial_bit_offset = bit_offset % 8;

	no_of_bits = 0;
	value      = 0;
	i          = 0;
	mask_greatest_bit_offset = 0;
	composite_bitmask        = 0;
	composite_bitmap         = 0;

	while (crumb_spec[i].crumb_bit_length != 0) {
		guint64 crumb_mask, crumb_value;
		guint8	crumb_end_bit_offset;

		DISSECTOR_ASSERT(i < 64);
		crumb_value = tvb_get_bits64(tvb,
					     bit_offset + crumb_spec[i].crumb_bit_offset,
					     crumb_spec[i].crumb_bit_length,
					     ENC_BIG_ENDIAN);
		value      += crumb_value;
		no_of_bits += crumb_spec[i].crumb_bit_length;

		/
		if (mask_greatest_bit_offset < 32) {
			crumb_end_bit_offset = mask_initial_bit_offset
				+ crumb_spec[i].crumb_bit_offset
				+ crumb_spec[i].crumb_bit_length;
			crumb_mask = (1 << crumb_spec[i].crumb_bit_length) - 1;

			if (crumb_end_bit_offset > mask_greatest_bit_offset) {
				mask_greatest_bit_offset = crumb_end_bit_offset;
			}
			composite_bitmask |= (crumb_mask  << (64 - crumb_end_bit_offset));
			composite_bitmap  |= (crumb_value << (64 - crumb_end_bit_offset));
		}
		/
		value <<= crumb_spec[++i].crumb_bit_length;
	}

	/
	switch (hf_field->type) {
		case FT_INT8:
		case FT_INT16:
		case FT_INT24:
		case FT_INT32:
		case FT_INT64:
			if (value & no_of_bits && (G_GINT64_CONSTANT(1) << (no_of_bits-1)))
				value |= (G_GINT64_CONSTANT(-1) << no_of_bits);
			break;
		default:
			break;
	}

	if (return_value) {
		*return_value = value;
	}

	/
	TRY_TO_FAKE_THIS_ITEM(tree, hf_index, hf_field);

	/
	bf_str    = ep_alloc(256);
	bf_str[0] = '\0';

	octet_offset = bit_offset >> 3;

	/
	octet_length = ((mask_greatest_bit_offset + 7) >> 3);
	mask_greatest_bit_offset = octet_length << 3;

	/
	if (mask_greatest_bit_offset <= 32) {
		other_decode_bitfield_value(bf_str,
					    (guint32)(composite_bitmap  >> (64 - mask_greatest_bit_offset)),
					    (guint32)(composite_bitmask >> (64 - mask_greatest_bit_offset)),
					    mask_greatest_bit_offset);
	}

	switch (hf_field->type) {
	case FT_BOOLEAN: /
		/
		tfstring = (const true_false_string *) &tfs_true_false;
		if (hf_field->strings)
			tfstring = (const true_false_string *) hf_field->strings;
		return proto_tree_add_boolean_format(tree, hf_index,
						     tvb, octet_offset, octet_length, (guint32)value,
						     "%s = %s: %s",
						     bf_str, hf_field->name,
						     (guint32)value ? tfstring->true_string : tfstring->false_string);
		break;

	case FT_UINT8:
	case FT_UINT16:
	case FT_UINT24:
	case FT_UINT32:
		pi = proto_tree_add_uint(tree, hf_index, tvb, octet_offset, octet_length, (guint32)value);
		fill_label_uint(PITEM_FINFO(pi), lbl_str);
		break;

	case FT_INT8:
	case FT_INT16:
	case FT_INT24:
	case FT_INT32:
		pi = proto_tree_add_int(tree, hf_index, tvb, octet_offset, octet_length, (gint32)value);
		fill_label_int(PITEM_FINFO(pi), lbl_str);
		break;

	case FT_UINT64:
		pi = proto_tree_add_uint64(tree, hf_index, tvb, octet_offset, octet_length, value);
		fill_label_uint64(PITEM_FINFO(pi), lbl_str);
		break;

	case FT_INT64:
		pi = proto_tree_add_int64(tree, hf_index, tvb, octet_offset, octet_length, (gint64)value);
		fill_label_int64(PITEM_FINFO(pi), lbl_str);
		break;

	default:
		DISSECTOR_ASSERT_NOT_REACHED();
		return NULL;
		break;
	}
	proto_item_set_text(pi, "%s = %s", bf_str, lbl_str);
	return pi;
}
