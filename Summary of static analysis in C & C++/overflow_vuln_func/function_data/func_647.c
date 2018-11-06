int
wtap_register_file_type_subtypes(const struct file_type_subtype_info* fi, const int subtype)
{
	struct file_type_subtype_info* finfo = NULL;
	init_file_types_subtypes();

	if (!fi || !fi->name || !fi->short_name || subtype > wtap_num_file_types_subtypes) {
		g_error("no file type info or invalid file type to register");
		return subtype;
	}

	/
	if (subtype == WTAP_FILE_TYPE_SUBTYPE_UNKNOWN) {
		/
		if (wtap_short_string_to_file_type_subtype(fi->short_name) > -1 ) {
			g_error("file type short name already exists");
			return subtype;
		}

		g_array_append_val(dump_open_table_arr,*fi);

		dump_open_table = (const struct file_type_subtype_info*)(void *)dump_open_table_arr->data;

		return wtap_num_file_types_subtypes++;
	}

	/
	if (!dump_open_table[subtype].short_name || strcmp(dump_open_table[subtype].short_name,fi->short_name) != 0) {
		g_error("invalid file type name given to register");
		return subtype;
	}

	/
	finfo = (struct file_type_subtype_info*)(&dump_open_table[subtype]);
	/
	/
	finfo->default_file_extension     = fi->default_file_extension;
	finfo->additional_file_extensions = fi->additional_file_extensions;
	finfo->writing_must_seek          = fi->writing_must_seek;
	finfo->has_name_resolution        = fi->has_name_resolution;
	finfo->supported_comment_types    = fi->supported_comment_types;
	finfo->can_write_encap            = fi->can_write_encap;
	finfo->dump_open                  = fi->dump_open;
	finfo->wslua_info                 = fi->wslua_info;

	return subtype;
}
