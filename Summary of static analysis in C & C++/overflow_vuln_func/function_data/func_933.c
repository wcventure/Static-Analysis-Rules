static char* wslua_get_actual_filename(const char* fname) {
	char fname_clean[256];
	char* f;
	char* filename;
	
	strncpy(fname_clean,fname,256);
	
	for(f = fname_clean; *f; f++) {
		switch(*f) {
			case '/': case '\\':
				*f = *(G_DIR_SEPARATOR_S);
				break;
			default:
				break;
		}
	}
		
	if ( file_exists(fname_clean) ) {
		return g_strdup(fname_clean);
	}
	
	filename = get_persconffile_path(fname_clean,FALSE,FALSE);
	
	if ( file_exists(filename) ) {
		return filename;
	}
	g_free(filename);
	
	filename = get_datafile_path(fname_clean);
	if ( file_exists(filename) ) {
		return filename;
	}
	g_free(filename);

	return NULL;
}
