static char *airpcap_conf_path = NULL;
const char *ws80211_get_helper_path(void)
{
	HKEY h_key = NULL;

	if (!airpcap_conf_path && RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\AirPcap"), 0, KEY_QUERY_VALUE|KEY_WOW64_32KEY, &h_key) == ERROR_SUCCESS) {
		DWORD reg_ret;
		TCHAR airpcap_dir_utf16[MAX_PATH];
		DWORD ad_size = sizeof(airpcap_dir_utf16);

		reg_ret = RegQueryValueEx(h_key, NULL, NULL, NULL,
				(LPBYTE) &airpcap_dir_utf16, &ad_size);

		if (reg_ret == ERROR_SUCCESS) {
			airpcap_dir_utf16[ad_size] = L'\0';
			g_free(airpcap_conf_path);
			airpcap_conf_path = g_strdup_printf("%s\\AirpcapConf.exe", utf_16to8(airpcap_dir_utf16));

			if (!g_file_test(airpcap_conf_path, G_FILE_TEST_IS_EXECUTABLE)) {
				g_free(airpcap_conf_path);
				airpcap_conf_path = NULL;
			}
		}
	}

	return airpcap_conf_path;
}
