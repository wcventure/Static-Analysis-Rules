void
init_plugins()
{
    struct stat std_dir_stat, local_dir_stat, plugin_dir_stat;

    if (plugin_list == NULL)      /
    {
	enabled_plugins_number = 0;

#ifdef PLUGINS_NEED_ADDRESS_TABLE
	/
	patable.p_check_col			= check_col;
	patable.p_col_add_fstr			= col_add_fstr;
	patable.p_col_append_fstr		= col_append_str;
	patable.p_col_add_str			= col_add_str;
	patable.p_col_append_str		= col_append_str;

	patable.p_dfilter_init			= dfilter_init;
	patable.p_dfilter_cleanup		= dfilter_cleanup;

	patable.p_pi				= &pi;

	patable.p_proto_register_protocol	= proto_register_protocol;
	patable.p_proto_register_field_array	= proto_register_field_array;
	patable.p_proto_register_subtree_array	= proto_register_subtree_array;

	patable.p_dissector_add			= dissector_add;

	patable.p_heur_dissector_add		= heur_dissector_add;

	patable.p_proto_item_add_subtree	= proto_item_add_subtree;
	patable.p_proto_tree_add_item		= proto_tree_add_item;
	patable.p_proto_tree_add_item_hidden	= proto_tree_add_item_hidden;
	patable.p_proto_tree_add_protocol_format = proto_tree_add_protocol_format;
	patable.p_proto_tree_add_bytes		= proto_tree_add_bytes;
	patable.p_proto_tree_add_bytes_hidden	= proto_tree_add_bytes_hidden;
	patable.p_proto_tree_add_bytes_format	= proto_tree_add_bytes_format;
	patable.p_proto_tree_add_time		= proto_tree_add_time;
	patable.p_proto_tree_add_time_hidden	= proto_tree_add_time_hidden;
	patable.p_proto_tree_add_time_format	= proto_tree_add_time_format;
	patable.p_proto_tree_add_ipxnet		= proto_tree_add_ipxnet;
	patable.p_proto_tree_add_ipxnet_hidden	= proto_tree_add_ipxnet_hidden;
	patable.p_proto_tree_add_ipxnet_format	= proto_tree_add_ipxnet_format;
	patable.p_proto_tree_add_ipv4		= proto_tree_add_ipv4;
	patable.p_proto_tree_add_ipv4_hidden	= proto_tree_add_ipv4_hidden;
	patable.p_proto_tree_add_ipv4_format	= proto_tree_add_ipv4_format;
	patable.p_proto_tree_add_ipv6		= proto_tree_add_ipv6;
	patable.p_proto_tree_add_ipv6_hidden	= proto_tree_add_ipv6_hidden;
	patable.p_proto_tree_add_ipv6_format	= proto_tree_add_ipv6_format;
	patable.p_proto_tree_add_ether		= proto_tree_add_ether;
	patable.p_proto_tree_add_ether_hidden	= proto_tree_add_ether_hidden;
	patable.p_proto_tree_add_ether_format	= proto_tree_add_ether_format;
	patable.p_proto_tree_add_string		= proto_tree_add_string;
	patable.p_proto_tree_add_string_hidden	= proto_tree_add_string_hidden;
	patable.p_proto_tree_add_string_format	= proto_tree_add_string_format;
	patable.p_proto_tree_add_boolean	= proto_tree_add_boolean;
	patable.p_proto_tree_add_boolean_hidden	= proto_tree_add_boolean_hidden;
	patable.p_proto_tree_add_boolean_format	= proto_tree_add_boolean_format;
	patable.p_proto_tree_add_double		= proto_tree_add_double;
	patable.p_proto_tree_add_double_hidden	= proto_tree_add_double_hidden;
	patable.p_proto_tree_add_double_format	= proto_tree_add_double_format;
	patable.p_proto_tree_add_uint		= proto_tree_add_uint;
	patable.p_proto_tree_add_uint_hidden	= proto_tree_add_uint_hidden;
	patable.p_proto_tree_add_uint_format	= proto_tree_add_uint_format;
	patable.p_proto_tree_add_int		= proto_tree_add_int;
	patable.p_proto_tree_add_int_hidden	= proto_tree_add_int_hidden;
	patable.p_proto_tree_add_int_format	= proto_tree_add_int_format;
	patable.p_proto_tree_add_text		= proto_tree_add_text;
	patable.p_proto_tree_add_notext		= proto_tree_add_notext;
#endif

	plugins_scan_dir(std_plug_dir);
	plugins_scan_dir(local_plug_dir);
	if ((strcmp(std_plug_dir, PLUGIN_DIR) != 0) &&
		(strcmp(local_plug_dir, PLUGIN_DIR) != 0))
	{
	    if (stat(PLUGIN_DIR, &plugin_dir_stat) == 0)
	    {
		/
		if (stat(std_plug_dir, &std_dir_stat) == 0)
		{
		    if (stat(local_plug_dir, &local_dir_stat) == 0)
		    {
			if ((plugin_dir_stat.st_dev != std_dir_stat.st_dev ||
				    plugin_dir_stat.st_ino != std_dir_stat.st_ino) &&
				(plugin_dir_stat.st_dev != local_dir_stat.st_dev ||
				 plugin_dir_stat.st_ino != local_dir_stat.st_ino))
			    plugins_scan_dir(PLUGIN_DIR);
		    }
		    else
		    {
			if ((plugin_dir_stat.st_dev != std_dir_stat.st_dev ||
				    plugin_dir_stat.st_ino != std_dir_stat.st_ino))
			    plugins_scan_dir(PLUGIN_DIR);
		    }
		}
		else if (stat(local_plug_dir, &local_dir_stat) == 0)
		{
		    if ((plugin_dir_stat.st_dev != local_dir_stat.st_dev ||
				plugin_dir_stat.st_ino != local_dir_stat.st_ino))
			plugins_scan_dir(PLUGIN_DIR);
		}
		else plugins_scan_dir(PLUGIN_DIR);
	    }
	}
	if (!user_plug_dir)
	{
	    user_plug_dir = (gchar *)g_malloc(strlen(get_home_dir()) + 19);
	    sprintf(user_plug_dir, "%s/%s/plugins", get_home_dir(), PF_DIR);
	}
	plugins_scan_dir(user_plug_dir);
    }
}
