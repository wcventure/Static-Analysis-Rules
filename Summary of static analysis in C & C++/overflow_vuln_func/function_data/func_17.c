static void
decode_evt_error(proto_tree *tree, tvbuff_t *tvb, guint offset, guint length)
{
	guint8  method, class, property;
	guint16 errcode;
	guint32 Pdata;

	errcode = tvb_get_ntohs (tvb, offset);
	proto_tree_add_uint_format_value(tree, hf_noe_errcode, tvb, offset, 2,
		errcode, "%s (%d)", val_to_str(errcode, VALS(errcode_vals), "Unknown"), errcode);
	offset += 2;
	length -= 2;

	method = tvb_get_guint8(tvb, offset);
	proto_tree_add_uint_format_value(tree, hf_noe_method, tvb, offset, 1,
		method, "%s (%d)", val_to_str(method, VALS(methods_vals), "Unknown"), method);
	offset ++;
	length --;

	class = tvb_get_guint8(tvb, offset);
	proto_tree_add_int_format_value(tree, hf_noe_class, tvb, offset, 1,
		class, "%s (%d)", val_to_str(class, val_str_class, "Unknown"), class);
	offset ++;
	length --;

	proto_tree_add_uint(tree, hf_noe_objectid, tvb, offset, 2, tvb_get_ntohs(tvb, offset));
	offset += 2;
	length -= 2;

	property = tvb_get_guint8(tvb, offset);
	proto_tree_add_int_format_value(tree, hf_noe_pcode, tvb, offset, 1,
		property, "%s (0x%02x)", val_to_str(property, val_str_props, "Unknown"), property);
	offset ++;
	length --;

	proto_tree_add_item(tree, hf_noe_aindx, tvb, offset, 1, ENC_NA);
	offset ++;
	length --;

	proto_tree_add_uint(tree, hf_noe_length, tvb, offset, 2, tvb_get_ntohs(tvb, offset));
	offset += 2;
	length -= 2;

	Pdata = tvb_get_ntohl (tvb, offset);
	proto_tree_add_text(tree,
		tvb,
		offset,
		4,
		"Value: %x",
		Pdata);
	offset += 4;
	length -= 4;

	proto_tree_add_text(tree,
		tvb,
		offset,
		length,
		"Message: %s",
		tvb_bytes_to_str(tvb, offset, length));
}
