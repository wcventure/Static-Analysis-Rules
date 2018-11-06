proto_item *
proto_tree_add_bits_format_value(proto_tree *tree, const int hf_index,
				 tvbuff_t *tvb, const gint bit_offset,
				 const gint no_of_bits, void *value_ptr,
				 gchar *value_str)
{
	proto_item *item;

	if ((item = _proto_tree_add_bits_format_value(tree, hf_index,
						      tvb, bit_offset, no_of_bits,
						      value_ptr, value_str))) {
		FI_SET_FLAG(PNODE_FINFO(item), FI_BITS_OFFSET(bit_offset));
		FI_SET_FLAG(PNODE_FINFO(item), FI_BITS_SIZE(no_of_bits));
	}
	return item;
}
