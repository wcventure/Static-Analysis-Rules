int
save_plugin_status()
{
    gchar  *pf_path;
    FILE   *statusfile;
    plugin *pt_plug;

    if (!plugin_status_file) {
	plugin_status_file = (gchar *)g_malloc(strlen(get_home_dir()) + 26);
	sprintf(plugin_status_file, "%s/%s/plugins.status", get_home_dir(), PF_DIR);
    }
    statusfile=fopen(plugin_status_file, "w");
    if (!statusfile) {
	pf_path = g_malloc(strlen(get_home_dir()) + strlen(PF_DIR) + 2);
	sprintf(pf_path, "%s/%s", get_home_dir(), PF_DIR);
#ifdef WIN32
	mkdir(pf_path);
#else
	mkdir(pf_path, 0755);
#endif
	g_free(pf_path);
	statusfile=fopen(plugin_status_file, "w");
	if (!statusfile) return -1;
    }

    pt_plug = plugin_list;
    while (pt_plug)
    {
	fprintf(statusfile,"%s %s %s\n%s\n", pt_plug->name, pt_plug->version,
		(pt_plug->enabled ? "1" : "0"), pt_plug->filter_string);
	pt_plug = pt_plug->next;
    }
    fclose(statusfile);
    return 0;
}
