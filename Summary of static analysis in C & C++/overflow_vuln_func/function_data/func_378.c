static void
plugins_scan_dir(const char *dirname)
{
    DIR           *dir;             /
    struct dirent *file;            /
    gchar          filename[512];   /
    GModule       *handle;          /
    gchar         *name;
    gchar         *version;
    gchar         *protocol;
    gchar         *filter_string;
    gchar         *dot;
    dfilter       *filter = NULL;
    void         (*dissector) (const u_char *, int, frame_data *, proto_tree *);
    int            cr;
    FILE          *statusfile;

#ifdef WIN32
#define LT_LIB_EXT ".dll"
#else
#define LT_LIB_EXT ".so"
#endif

    if (!plugin_status_file)
    {
	plugin_status_file = (gchar *)g_malloc(strlen(get_home_dir()) + 26);
	sprintf(plugin_status_file, "%s/%s/plugins.status", get_home_dir(), PF_DIR);
    }
    statusfile = fopen(plugin_status_file, "r");

    if ((dir = opendir(dirname)) != NULL)
    {
	while ((file = readdir(dir)) != NULL)
	{
	    /
	    if (!(strcmp(file->d_name, "..") &&
		  strcmp(file->d_name, "."))) continue;

            /
            dot = strrchr(file->d_name, '.');
            if (dot == NULL || strcmp(dot, LT_LIB_EXT) != 0) continue;

	    sprintf(filename, "%s/%s", dirname, file->d_name);
	    if ((handle = g_module_open(filename, 0)) == NULL) continue;
	    name = (gchar *)file->d_name;
	    if (g_module_symbol(handle, "version", (gpointer*)&version) == FALSE)
	    {
	        g_warning("The plugin %s has no version symbol", name);
		g_module_close(handle);
		continue;
	    }
	    if (g_module_symbol(handle, "protocol", (gpointer*)&protocol) == FALSE)
	    {
	        g_warning("The plugin %s has no protocol symbol", name);
		g_module_close(handle);
		continue;
	    }
	    if (g_module_symbol(handle, "filter_string", (gpointer*)&filter_string) == FALSE)
	    {
	        g_warning("The plugin %s has no filter_string symbol", name);
		g_module_close(handle);
		continue;
	    }
	    if (dfilter_compile(filter_string, &filter) != 0) {
	        g_warning("The plugin %s has a non compilable filter", name);
		g_module_close(handle);
		continue;
	    }
	    if (g_module_symbol(handle, "dissector", (gpointer*)&dissector) == FALSE) {
		if (filter != NULL)
		    dfilter_destroy(filter);
	        g_warning("The plugin %s has no dissector symbol", name);
		g_module_close(handle);
		continue;
	    }

	    if ((cr = add_plugin(handle, g_strdup(file->d_name), version,
				 protocol, filter_string, filter, dissector)))
	    {
		if (cr == EEXIST)
		    fprintf(stderr, "The plugin : %s, version %s\n"
			    "was found in multiple directories\n", name, version);
		else
		    fprintf(stderr, "Memory allocation problem\n"
			    "when processing plugin %s, version %sn",
			    name, version);
		if (filter != NULL)
		    dfilter_destroy(filter);
		g_module_close(handle);
		continue;
	    }
	    if (statusfile) {
		check_plugin_status(file->d_name, version, handle,
			            filter_string, statusfile);
		rewind(statusfile);
	    }
	}
	closedir(dir);
    }
    if (statusfile) fclose(statusfile);
}
