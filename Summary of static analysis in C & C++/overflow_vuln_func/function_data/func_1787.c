static gint
byte_view_select(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	GtkCTree	*ctree = GTK_CTREE(tree_view);
	GtkCTreeNode	*node, *parent;
	field_info	*finfo;
	GtkText		*bv = GTK_TEXT(widget);
	int		row, column;
	int		byte;

	/
	const int	digits_start_1 = 6;
	/
	const int	digits_end_1 = 28;

	/
	const int	digits_start_2 = 31;
	/
	const int	digits_end_2 = 53;

	/
	const int	text_start = 57;
	/
	const int	text_end = 73;

	/
	column = (bv->hadj->value + event->x) / m_font_width;
	row = (bv->vadj->value + event->y) / m_font_height;

	/
	if (column >= digits_start_1 && column <= digits_end_1) {
		byte = byte_num(column, digits_start_1);
		if (byte == -1) {
			return FALSE;
		}
	}
	else if (column >= digits_start_2 && column <= digits_end_2) {
		byte = byte_num(column, digits_start_2);
		if (byte == -1) {
			return FALSE;
		}
		byte += 8;
	}
	else if (column >= text_start && column <= text_end) {
		byte = column - text_start;
	}
	else {
		/
		return FALSE;
	}

	/
	byte += row * 16;


	/
	finfo = proto_find_field_from_offset(cfile.protocol_tree, byte);

	if (!finfo) {
		return FALSE;
	}

	node = gtk_ctree_find_by_row_data(ctree, NULL, finfo);
	g_assert(node);

	/
	gtk_ctree_expand(ctree, node);
	gtk_ctree_select(ctree, node);
	expand_tree(ctree, node, NULL);

	/
	parent = GTK_CTREE_ROW(node)->parent;
	while (parent) {
		gtk_ctree_expand(ctree, parent);
		expand_tree(ctree, parent, NULL);
		parent = GTK_CTREE_ROW(parent)->parent;
	}

	/
	gtk_ctree_node_moveto(ctree, node, 0, .5, 0);

	return FALSE;
}
