static void
delete_color_filter_it(gpointer filter_arg, gpointer ignored _U_)
{
	color_filter_t *colorf = filter_arg;
	
	delete_color_filter(colorf);
}
