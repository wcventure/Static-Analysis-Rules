proto_item*
ptvcursor_add(ptvcursor_t *ptvc, int hf, gint length, gboolean endianness)
{
	proto_item	*item;

	item = proto_tree_add_item(ptvc->tree, hf, ptvc->tvb, ptvc->offset,
			length, endianness);

	if (length == PTVC_VARIABLE_LENGTH) {
		ptvc->offset += proto_item_get_len(item);
	}
	else {
		ptvc->offset += length;
	}
	return item;
}
