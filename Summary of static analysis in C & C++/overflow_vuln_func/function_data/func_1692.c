static prefs_set_pref_e
read_set_recent_pair_static(gchar *key, const gchar *value,
                            void *private_data _U_,
                            gboolean return_range_errors _U_)
{
  long num;
  char *p;
  GList *col_l, *col_l_elt;
  col_width_data *cfmt;
  const gchar *cust_format = col_format_to_string(COL_CUSTOM);
  int cust_format_len = (int) strlen(cust_format);

  if (strcmp(key, RECENT_KEY_MAIN_TOOLBAR_SHOW) == 0) {
    if (g_ascii_strcasecmp(value, "true") == 0) {
        recent.main_toolbar_show = TRUE;
    }
    else {
        recent.main_toolbar_show = FALSE;
    }
  } else if (strcmp(key, RECENT_KEY_FILTER_TOOLBAR_SHOW) == 0) {
    if (g_ascii_strcasecmp(value, "true") == 0) {
        recent.filter_toolbar_show = TRUE;
    }
    else {
        recent.filter_toolbar_show = FALSE;
    }
  /
  } else if (strcmp(key, RECENT_KEY_WIRELESS_TOOLBAR_SHOW) == 0 || (strcmp(key, "gui.airpcap_toolbar_show") == 0)) {
    if (g_ascii_strcasecmp(value, "true") == 0) {
        recent.wireless_toolbar_show = TRUE;
    }
    else {
        recent.wireless_toolbar_show = FALSE;
    }
  } else if (strcmp(key, RECENT_KEY_DRIVER_CHECK_SHOW) == 0) {
    if (g_ascii_strcasecmp(value, "true") == 0) {
        recent.airpcap_driver_check_show = TRUE;
    }
    else {
        recent.airpcap_driver_check_show = FALSE;
    }
  } else if (strcmp(key, RECENT_KEY_PACKET_LIST_SHOW) == 0) {
    if (g_ascii_strcasecmp(value, "true") == 0) {
        recent.packet_list_show = TRUE;
    }
    else {
        recent.packet_list_show = FALSE;
    }
  } else if (strcmp(key, RECENT_KEY_TREE_VIEW_SHOW) == 0) {
    if (g_ascii_strcasecmp(value, "true") == 0) {
        recent.tree_view_show = TRUE;
    }
    else {
        recent.tree_view_show = FALSE;
    }
  } else if (strcmp(key, RECENT_KEY_BYTE_VIEW_SHOW) == 0) {
    if (g_ascii_strcasecmp(value, "true") == 0) {
        recent.byte_view_show = TRUE;
    }
    else {
        recent.byte_view_show = FALSE;
    }
  } else if (strcmp(key, RECENT_KEY_STATUSBAR_SHOW) == 0) {
    if (g_ascii_strcasecmp(value, "true") == 0) {
        recent.statusbar_show = TRUE;
    }
    else {
        recent.statusbar_show = FALSE;
    }
  } else if (strcmp(key, RECENT_KEY_PACKET_LIST_COLORIZE) == 0) {
    if (g_ascii_strcasecmp(value, "true") == 0) {
        recent.packet_list_colorize = TRUE;
    }
    else {
        recent.packet_list_colorize = FALSE;
    }
  } else if (strcmp(key, RECENT_GUI_TIME_FORMAT) == 0) {
    recent.gui_time_format =
      (ts_type)find_index_from_string_array(value, ts_type_text, TS_RELATIVE);
  } else if (strcmp(key, RECENT_GUI_TIME_PRECISION) == 0) {
    recent.gui_time_precision =
      find_index_from_string_array(value, ts_precision_text, TS_PREC_AUTO);
  } else if (strcmp(key, RECENT_GUI_SECONDS_FORMAT) == 0) {
    recent.gui_seconds_format =
      (ts_seconds_type)find_index_from_string_array(value, ts_seconds_text, TS_SECONDS_DEFAULT);
  } else if (strcmp(key, RECENT_GUI_ZOOM_LEVEL) == 0) {
    num = strtol(value, &p, 0);
    if (p == value || *p != '\0')
      return PREFS_SET_SYNTAX_ERR;      /
    recent.gui_zoom_level = (gint)num;
  } else if (strcmp(key, RECENT_GUI_BYTES_VIEW) == 0) {
    num = strtol(value, &p, 0);
    if (p == value || *p != '\0')
      return PREFS_SET_SYNTAX_ERR;      /
    recent.gui_bytes_view = (gint)num;
  } else if (strcmp(key, RECENT_GUI_GEOMETRY_MAIN_MAXIMIZED) == 0) {
    if (g_ascii_strcasecmp(value, "true") == 0) {
        recent.gui_geometry_main_maximized = TRUE;
    }
    else {
        recent.gui_geometry_main_maximized = FALSE;
    }

  } else if (strcmp(key, RECENT_GUI_GEOMETRY_MAIN_UPPER_PANE) == 0) {
    num = strtol(value, &p, 0);
    if (p == value || *p != '\0')
      return PREFS_SET_SYNTAX_ERR;      /
    if (num <= 0)
      return PREFS_SET_SYNTAX_ERR;      /
    recent.gui_geometry_main_upper_pane = (gint)num;
    recent.has_gui_geometry_main_upper_pane = TRUE;
  } else if (strcmp(key, RECENT_GUI_GEOMETRY_MAIN_LOWER_PANE) == 0) {
    num = strtol(value, &p, 0);
    if (p == value || *p != '\0')
      return PREFS_SET_SYNTAX_ERR;      /
    if (num <= 0)
      return PREFS_SET_SYNTAX_ERR;      /
    recent.gui_geometry_main_lower_pane = (gint)num;
    recent.has_gui_geometry_main_lower_pane = TRUE;
  } else if (strcmp(key, RECENT_GUI_CONVERSATION_TABS) == 0) {
    recent.conversation_tabs = prefs_get_string_list(value);
  } else if (strcmp(key, RECENT_GUI_ENDPOINT_TABS) == 0) {
    recent.endpoint_tabs = prefs_get_string_list(value);
  } else if (strcmp(key, RECENT_GUI_RLC_PDUS_FROM_MAC_FRAMES) == 0) {
    if (g_ascii_strcasecmp(value, "true") == 0) {
        recent.gui_rlc_use_pdus_from_mac = TRUE;
    }
    else {
        recent.gui_rlc_use_pdus_from_mac = FALSE;
    }
  } else if (strcmp(key, RECENT_KEY_COL_WIDTH) == 0) {
    col_l = prefs_get_string_list(value);
    if (col_l == NULL)
      return PREFS_SET_SYNTAX_ERR;
    if ((g_list_length(col_l) % 2) != 0) {
      /
      prefs_clear_string_list(col_l);
      return PREFS_SET_SYNTAX_ERR;
    }
    /
    col_l_elt = g_list_first(col_l);
    while (col_l_elt) {
      /
      if (strcmp((const char *)col_l_elt->data, "") == 0) {
        /
        prefs_clear_string_list(col_l);
        return PREFS_SET_SYNTAX_ERR;
      }

      /
      if (strncmp((const char *)col_l_elt->data, cust_format, cust_format_len) != 0) {
        if (get_column_format_from_str((const gchar *)col_l_elt->data) == -1) {
          /
          prefs_clear_string_list(col_l);
          return PREFS_SET_SYNTAX_ERR;
        }
      }

      /
      col_l_elt = col_l_elt->next;

      /
      col_l_elt = col_l_elt->next;
    }
    free_col_width_info(&recent);
    recent.col_width_list = NULL;
    col_l_elt = g_list_first(col_l);
    while (col_l_elt) {
      gchar *fmt = g_strdup((const gchar *)col_l_elt->data);
      cfmt = (col_width_data *) g_malloc(sizeof(col_width_data));
      if (strncmp(fmt, cust_format, cust_format_len) != 0) {
        cfmt->cfmt   = get_column_format_from_str(fmt);
        cfmt->cfield = NULL;
      } else {
        cfmt->cfmt   = COL_CUSTOM;
        cfmt->cfield = g_strdup(&fmt[cust_format_len+1]);  /
      }
      g_free (fmt);
      if (cfmt->cfmt == -1) {
        g_free(cfmt->cfield);
        g_free(cfmt);
        return PREFS_SET_SYNTAX_ERR;   /
      }

      col_l_elt      = col_l_elt->next;
      cfmt->width    = (gint)strtol((const char *)col_l_elt->data, &p, 0);
      if (p == col_l_elt->data || (*p != '\0' && *p != ':')) {
        g_free(cfmt->cfield);
        g_free(cfmt);
        return PREFS_SET_SYNTAX_ERR;    /
      }

      if (*p == ':') {
        cfmt->xalign = *(++p);
      } else {
        cfmt->xalign = COLUMN_XALIGN_DEFAULT;
      }

      col_l_elt      = col_l_elt->next;
      recent.col_width_list = g_list_append(recent.col_width_list, cfmt);
    }
    prefs_clear_string_list(col_l);
  } else if (strcmp(key, RECENT_GUI_FILEOPEN_REMEMBERED_DIR) == 0) {
    if (recent.gui_fileopen_remembered_dir) {
      g_free (recent.gui_fileopen_remembered_dir);
    }
    recent.gui_fileopen_remembered_dir = g_strdup(value);
  }

  return PREFS_SET_OK;
}
