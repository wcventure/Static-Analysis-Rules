static gint fill_tpncp_id_vals(value_string string[], FILE *file) {
    gint i = 0, tpncp_id = 0;
    gchar *tpncp_name = NULL, *line_in_file = NULL;

    line_in_file = ep_alloc(MAX_TPNCP_DB_ENTRY_LEN);
    line_in_file[0] = 0;
    tpncp_name = ep_alloc(MAX_TPNCP_DB_ENTRY_LEN);
    tpncp_name[0] = 0;

    while (fgets(line_in_file, MAX_TPNCP_DB_ENTRY_LEN, file) != NULL) {
        if (!strncmp(line_in_file, "#####", 5)) {
            break;
        }
        if (sscanf(line_in_file, "%s %d", tpncp_name, &tpncp_id) == 2) {
            string[i].strptr = g_strdup(tpncp_name);
            string[i].value = tpncp_id;
            if (i < MAX_TPNCP_DB_SIZE) {
                i++;
            }
            else {
                break;
            }
        }
    }

    return 0;
}
