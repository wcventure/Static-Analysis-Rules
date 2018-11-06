gboolean
revert_filters(void)
{
	gchar *pf_dir_path;
	const gchar *path;

	/
	if (create_persconffile_dir(&pf_dir_path) == -1) {
		simple_dialog(ESD_TYPE_ERROR, ESD_BTN_OK,
		    "Can't create directory\n\"%s\"\nfor color files: %s.",
		    pf_dir_path, strerror(errno));
		g_free(pf_dir_path);
		return FALSE;
	}

	path = get_persconffile_path("colorfilters", TRUE);
	if (!deletefile(path))
		return FALSE;

	/
	colfilter_init();
        return TRUE;
}
