static gboolean
check_for_offset(GNode *node, gpointer data)
{
	field_info		*fi = node->data;
	offset_search_t		*offsearch = data;

	/
	if (fi && fi->visible) {
		if (offsearch->offset >= fi->start &&
				offsearch->offset <= (fi->start + fi->length)) {

			offsearch->finfo = fi;
			return FALSE; /
		}
	}
	return FALSE; /
}
