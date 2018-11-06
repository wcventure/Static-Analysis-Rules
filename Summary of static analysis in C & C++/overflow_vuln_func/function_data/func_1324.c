{
	color_filter_t *colorf;
        GtkStyle       *style;

	colorf = (color_filter_t *)g_malloc(sizeof (color_filter_t));
	colorf->filter_name = g_strdup(name);
	colorf->filter_text = g_strdup(filter_string);
        style = gtk_widget_get_style(packet_list);
	gdkcolor_to_color_t(&colorf->bg_color, &style->base[GTK_STATE_NORMAL]);
	gdkcolor_to_color_t(&colorf->fg_color, &style->text[GTK_STATE_NORMAL]);
	colorf->c_colorfilter = NULL;
	colorf->edit_dialog = NULL;
	colorf->marked = FALSE;
	filter_list = g_slist_append(filter_list, colorf);
        return colorf;
}
