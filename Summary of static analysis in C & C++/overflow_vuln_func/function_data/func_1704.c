static gint fill_enums_id_vals(FILE *file) {
    gint i = 0, enum_id = 0, enum_val = 0, first_entry = 1;
    gchar *line_in_file = NULL, *enum_name = NULL,
           *enum_type = NULL, *enum_str = NULL;

    line_in_file = ep_alloc(MAX_TPNCP_DB_ENTRY_LEN);
    line_in_file[0] = 0;
    enum_name = ep_alloc(MAX_TPNCP_DB_ENTRY_LEN);
    enum_name[0] = 0;
    enum_type = ep_alloc(MAX_TPNCP_DB_ENTRY_LEN);
    enum_type[0] = 0;
    enum_str = ep_alloc(MAX_TPNCP_DB_ENTRY_LEN);
    enum_str[0] = 0;

    while (fgets(line_in_file, MAX_TPNCP_DB_ENTRY_LEN, file) != NULL) {
        if (!strncmp(line_in_file, "#####", 5)) {
            break;
        }
        if (sscanf(line_in_file, "%s %s %d", enum_name, enum_str, &enum_id) == 3) {
            if (strcmp(enum_type, enum_name)) {
                if (!first_entry) {
                    if (enum_val < MAX_ENUMS_NUM) {
                        tpncp_enums_id_vals[enum_val][i].strptr = NULL;
                        tpncp_enums_id_vals[enum_val][i].value = 0;
                        enum_val++; i = 0;
                    }
                    else {
                        break;
                    }
                }
                else
                    first_entry = 0;
                tpncp_enums_name_vals[enum_val] = g_strdup(enum_name);
                g_strlcpy(enum_type, enum_name, MAX_TPNCP_DB_ENTRY_LEN);
            }
            tpncp_enums_id_vals[enum_val][i].strptr = g_strdup(enum_str);
            tpncp_enums_id_vals[enum_val][i].value = enum_id;
            if (i < MAX_ENUM_ENTRIES) {
                i++;
            }
            else {
                break;
            }
        }
    }

    return 0;
}
