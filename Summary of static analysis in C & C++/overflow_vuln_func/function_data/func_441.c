static
void proto_tree_print_node(proto_node *node, gpointer data)
{
	field_info	*fi = PITEM_FINFO(node);
	print_data	*pdata = (print_data*) data;
	const guint8	*pd;
	gchar		label_str[ITEM_LABEL_LENGTH];
	gchar		*label_ptr;

	/
	if (!fi->visible)
		return;

    /
	if (fi->rep) {
		label_ptr = fi->rep->representation;
	}
	else { /
		label_ptr = label_str;
		proto_item_fill_label(fi, label_str);
	}

	print_line(pdata->fh, pdata->level, pdata->format, label_ptr);

	/
	if (fi->hfinfo->id == proto_data && pdata->print_hex_for_data) {
		/
		pd = get_field_data(pdata->src_list, fi);
		print_hex_data_buffer(pdata->fh, pd, fi->length,
		    pdata->encoding, pdata->format);
	}

	/
	g_assert(fi->tree_type >= -1 && fi->tree_type < num_tree_types);
	if (pdata->print_dissections == print_dissections_expanded ||
	    (pdata->print_dissections == print_dissections_as_displayed &&
        fi->tree_type >= 0 && tree_is_expanded[fi->tree_type])) {
		if (node->first_child != NULL) {
			pdata->level++;
			proto_tree_children_foreach(node,
				proto_tree_print_node, pdata);
			pdata->level--;
		}
	}
}
