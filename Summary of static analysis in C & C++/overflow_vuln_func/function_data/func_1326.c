static gboolean
read_filters(void)
{
	/
	/
	/
	gchar *path;
	FILE *f;
	gboolean ret;

	/
	path = get_persconffile_path("colorfilters", FALSE);
	if ((f = fopen(path, "r")) == NULL) {
		if (errno != ENOENT) {
			simple_dialog(ESD_TYPE_ERROR, ESD_BTN_OK,
			    "Could not open filter file\n\"%s\": %s.", path,
			    strerror(errno));
		}
		g_free((gchar *)path);
		return FALSE;
	}
	g_free((gchar *)path);
	path = NULL;

	ret = read_filters_file(f, NULL);
	fclose(f);
	return ret;
}
