static const char *
get_persconffile_dir(const gchar *profilename)
{
	static char *persconffile_profile_dir = NULL;

	if (persconffile_profile_dir) {
		g_free (persconffile_profile_dir);
	}

	if (profilename && strlen(profilename) > 0 &&
	    strcmp(profilename, DEFAULT_PROFILE) != 0) {
	  persconffile_profile_dir = g_strdup_printf ("%s%s%s", get_profiles_dir (),
						      G_DIR_SEPARATOR_S, profilename);
	} else {
	  persconffile_profile_dir = g_strdup (get_persconffile_dir_no_profile ());
	}

	return persconffile_profile_dir;
}
