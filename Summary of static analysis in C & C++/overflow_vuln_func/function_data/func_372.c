static int
loop_record(tvbuff_t *tvb, proto_tree *ptree, gint offset,
		int count, guint16 d_bitmap, guint16 f_bitmap, int add)
{
  	proto_tree *tree = NULL;
  	proto_item *item;
	gchar 	*name;
	guint8	flags;
	guint8	size;
	gint	org;
	int i;

	for (i = 0; i < count; i++) {
		org = offset;
		name = NULL;
		size = tvb_get_guint8(tvb, offset) +add;
		flags = tvb_get_guint8(tvb, offset +1);

		if (ptree) {
			if (flags) {
				name = name_in_dbitmap(tvb, offset +2, d_bitmap);
			}
			else {
				name = name_in_fbitmap(tvb, offset +2, f_bitmap);
			}
			if (!name) {
				if (!(name = g_malloc(50))) { /
				}
				snprintf(name, 50,"line %d", i +1);
			}
			item = proto_tree_add_text(ptree, tvb, offset, size, name);
			tree = proto_item_add_subtree(item, ett_afp_enumerate_line);
		}
		proto_tree_add_item(tree, hf_afp_struct_size, tvb, offset, 1,FALSE);
		offset++;

		proto_tree_add_item(tree, hf_afp_file_flag, tvb, offset, 1,FALSE);
		offset++;
		if (flags) {
			offset = parse_dir_bitmap(tree, tvb, offset, d_bitmap);
		}
		else {
			offset = parse_file_bitmap(tree, tvb, offset, f_bitmap,0);
		}
		if ((offset & 1))
			PAD(1);
		offset = org +size;		/
		if (ptree)
			g_free((gpointer)name);
	}
	return offset;
}
