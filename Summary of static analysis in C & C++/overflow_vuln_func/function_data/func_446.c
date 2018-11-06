static
void proto_tree_get_node(GNode *node, gpointer data)
{
	field_info	*fi = PITEM_FINFO(node);
	print_data	*pdata = (print_data*) data;
	const guint8	*pd;
	gchar		label_str[ITEM_LABEL_LENGTH];
	gchar		*label_ptr, *string_ptr;

    string_ptr = decode_data;

	/
	if (!fi->visible)
		return;

	/
	if (fi->representation) {
		label_ptr = fi->representation;
	}
	else { /
		label_ptr = label_str;
		proto_item_fill_label(fi, label_str);
	}
    
    strcat(string_ptr, label_ptr);

    /
	pd = get_field_data(pdata->src_list, fi);
    if (pd!=NULL) {
        if (strlen(pd) > 0) {
            strcat(string_ptr, pd);
        }
    }

	/
	g_assert(fi->tree_type >= -1 && fi->tree_type < num_tree_types);
	if (pdata->print_all_levels ||
	    (fi->tree_type >= 0 && tree_is_expanded[fi->tree_type])) {
		if (g_node_n_children(node) > 0) {
			pdata->level++;
			g_node_children_foreach(node, G_TRAVERSE_ALL,
				proto_tree_get_node, pdata);
			pdata->level--;
		}
	}
}
