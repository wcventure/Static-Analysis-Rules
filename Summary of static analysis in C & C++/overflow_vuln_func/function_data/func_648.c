void
wtap_deregister_file_type_subtype(const int subtype)
{
	struct file_type_subtype_info* finfo = NULL;

	if (subtype < 0 || subtype >= wtap_num_file_types_subtypes) {
		g_error("invalid file type to de-register");
		return;
	}

	/
	finfo = (struct file_type_subtype_info*)(&dump_open_table[subtype]);
	/
	/
	/
	finfo->default_file_extension = NULL;
	finfo->additional_file_extensions = NULL;
	finfo->writing_must_seek = FALSE;
	finfo->has_name_resolution = FALSE;
	finfo->supported_comment_types = 0;
	finfo->can_write_encap = NULL;
	finfo->dump_open = NULL;
	finfo->wslua_info = NULL;
}
