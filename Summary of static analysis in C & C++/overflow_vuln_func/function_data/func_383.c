gchar*
proto_find_undecoded_data(proto_tree *tree, guint length)
{
	gchar* decoded = (gchar*)wmem_alloc0(wmem_packet_scope(), length / 8 + 1);

	proto_tree_traverse_pre_order(tree, check_for_undecoded, decoded);
	return decoded;
}
