void
delete_color_filter(color_filter_t *colorf)
{
	if (colorf->filter_name != NULL)
		g_free(colorf->filter_name);
	if (colorf->filter_text != NULL)
		g_free(colorf->filter_text);
	if (colorf->c_colorfilter != NULL)
		dfilter_free(colorf->c_colorfilter);
	filter_list = g_slist_remove(filter_list, colorf);
	g_free(colorf);
}
