static proto_item *
dissect_cbor_float_simple_data(tvbuff_t *tvb, packet_info *pinfo, proto_tree *cbor_tree, gint *offset, guint8 type_minor)
{
	proto_item      *item;
	int		half;

	switch (type_minor) {
	case 0x18:
		*offset += 1;
		item = proto_tree_add_item(cbor_tree, hf_cbor_type_simple_data8, tvb, *offset, 1, ENC_BIG_ENDIAN);
		*offset += 1;
		return item;
	case 0x19:
		*offset += 1;
		half = tvb_get_ntohs(tvb, *offset);
		item = proto_tree_add_float(cbor_tree, hf_cbor_type_float16, tvb, *offset, 2, decode_half(half));
		*offset += 2;
		return item;
	case 0x1a:
		*offset += 1;
		item = proto_tree_add_item(cbor_tree, hf_cbor_type_float32, tvb, *offset, 4, ENC_BIG_ENDIAN);
		*offset += 4;
		return item;
	case 0x1b:
		*offset += 1;
		item = proto_tree_add_item(cbor_tree, hf_cbor_type_float64, tvb, *offset, 8, ENC_BIG_ENDIAN);
		*offset += 8;
		return item;
	default:
		if (type_minor <= 0x17) {
			item = proto_tree_add_item(cbor_tree, hf_cbor_type_simple_datas, tvb, *offset, 1, ENC_BIG_ENDIAN);
			*offset += 1;
			return item;
		}
		expert_add_info_format(pinfo, cbor_tree, &ei_cbor_invalid_minor_type,
				"invalid minor type %i in fimple data and float", type_minor);
		return NULL;
	}
}
