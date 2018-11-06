static void decode_getproperty_tlv(proto_tree *tree, tvbuff_t *tvb, guint offset, guint length)
{
	proto_item *body_item;
	proto_tree *body_tree;
	guint8 body_type;

	/
	body_item = proto_tree_add_text(tree,
		tvb,
		offset,
		length,
		"NOE Message Body");
	body_tree = proto_item_add_subtree(body_item, ett_property);

	while(length > 0)
	{
		body_type = tvb_get_guint8(tvb, offset);
		proto_tree_add_int_format_value(body_tree, hf_noe_pcode, tvb, offset, 1,
			body_type, "%s (0x%02x)", val_to_str(body_type, val_str_props, "Unknown"), body_type);

		offset++;
		length--;

		if(body_type >= P_ARRAY)
		{
			proto_tree_add_item(body_item, hf_noe_aindx, tvb, offset, 1, ENC_NA);
			offset++;
			length--;
		}
	}
}
