gboolean
write_profile_recent(void)
{
  char        *pf_dir_path;
  char        *rf_path;
  char        *string_list;
  FILE        *rf;

  /

  /
  if (create_persconffile_dir(&pf_dir_path) == -1) {
     simple_dialog(ESD_TYPE_ERROR, ESD_BTN_OK,
      "Can't create directory\n\"%s\"\nfor recent file: %s.", pf_dir_path,
      g_strerror(errno));
     g_free(pf_dir_path);
     return FALSE;
  }

  rf_path = get_persconffile_path(RECENT_FILE_NAME, TRUE);
  if ((rf = ws_fopen(rf_path, "w")) == NULL) {
     simple_dialog(ESD_TYPE_ERROR, ESD_BTN_OK,
      "Can't open recent file\n\"%s\": %s.", rf_path,
      g_strerror(errno));
    g_free(rf_path);
    return FALSE;
  }
  g_free(rf_path);

  fputs("# Recent settings file for Wireshark " VERSION ".\n"
    "#\n"
    "# This file is regenerated each time Wireshark is quit\n"
    "# and when changing configuration profile.\n"
    "# So be careful, if you want to make manual changes here.\n"
    "\n", rf);

  fprintf(rf, "\n# Main Toolbar show (hide).\n");
  fprintf(rf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(rf, RECENT_KEY_MAIN_TOOLBAR_SHOW ": %s\n",
          recent.main_toolbar_show == TRUE ? "TRUE" : "FALSE");

  fprintf(rf, "\n# Filter Toolbar show (hide).\n");
  fprintf(rf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(rf, RECENT_KEY_FILTER_TOOLBAR_SHOW ": %s\n",
          recent.filter_toolbar_show == TRUE ? "TRUE" : "FALSE");

  fprintf(rf, "\n# Wireless Settings Toolbar show (hide).\n");
  fprintf(rf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(rf, RECENT_KEY_WIRELESS_TOOLBAR_SHOW ": %s\n",
          recent.wireless_toolbar_show == TRUE ? "TRUE" : "FALSE");

#ifdef HAVE_AIRPCAP
  fprintf(rf, "\n# Show (hide) old AirPcap driver warning dialog box.\n");
  fprintf(rf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(rf, RECENT_KEY_DRIVER_CHECK_SHOW ": %s\n",
          recent.airpcap_driver_check_show == TRUE ? "TRUE" : "FALSE");
#endif

  fprintf(rf, "\n# Packet list show (hide).\n");
  fprintf(rf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(rf, RECENT_KEY_PACKET_LIST_SHOW ": %s\n",
          recent.packet_list_show == TRUE ? "TRUE" : "FALSE");

  fprintf(rf, "\n# Tree view show (hide).\n");
  fprintf(rf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(rf, RECENT_KEY_TREE_VIEW_SHOW ": %s\n",
          recent.tree_view_show == TRUE ? "TRUE" : "FALSE");

  fprintf(rf, "\n# Byte view show (hide).\n");
  fprintf(rf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(rf, RECENT_KEY_BYTE_VIEW_SHOW ": %s\n",
          recent.byte_view_show == TRUE ? "TRUE" : "FALSE");

  fprintf(rf, "\n# Statusbar show (hide).\n");
  fprintf(rf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(rf, RECENT_KEY_STATUSBAR_SHOW ": %s\n",
          recent.statusbar_show == TRUE ? "TRUE" : "FALSE");

  fprintf(rf, "\n# Packet list colorize (hide).\n");
  fprintf(rf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(rf, RECENT_KEY_PACKET_LIST_COLORIZE ": %s\n",
          recent.packet_list_colorize == TRUE ? "TRUE" : "FALSE");

  fprintf(rf, "\n# Timestamp display format.\n");
  fprintf(rf, "# One of: RELATIVE, ABSOLUTE, ABSOLUTE_WITH_DATE, DELTA, DELTA_DIS, EPOCH, UTC, UTC_WITH_DATE\n");
  fprintf(rf, RECENT_GUI_TIME_FORMAT ": %s\n",
          ts_type_text[recent.gui_time_format]);

  fprintf(rf, "\n# Timestamp display precision.\n");
  fprintf(rf, "# One of: AUTO, SEC, DSEC, CSEC, MSEC, USEC, NSEC\n");
  fprintf(rf, RECENT_GUI_TIME_PRECISION ": %s\n",
          ts_precision_text[recent.gui_time_precision]);

  fprintf(rf, "\n# Seconds display format.\n");
  fprintf(rf, "# One of: SECONDS, HOUR_MIN_SEC\n");
  fprintf(rf, RECENT_GUI_SECONDS_FORMAT ": %s\n",
          ts_seconds_text[recent.gui_seconds_format]);

  fprintf(rf, "\n# Zoom level.\n");
  fprintf(rf, "# A decimal number.\n");
  fprintf(rf, RECENT_GUI_ZOOM_LEVEL ": %d\n",
          recent.gui_zoom_level);

  fprintf(rf, "\n# Bytes view.\n");
  fprintf(rf, "# A decimal number.\n");
  fprintf(rf, RECENT_GUI_BYTES_VIEW ": %d\n",
          recent.gui_bytes_view);

  fprintf(rf, "\n# Main window upper (or leftmost) pane size.\n");
  fprintf(rf, "# Decimal number.\n");
  if (recent.gui_geometry_main_upper_pane != 0) {
    fprintf(rf, RECENT_GUI_GEOMETRY_MAIN_UPPER_PANE ": %d\n",
            recent.gui_geometry_main_upper_pane);
  }
  fprintf(rf, "\n# Main window middle pane size.\n");
  fprintf(rf, "# Decimal number.\n");
  if (recent.gui_geometry_main_lower_pane != 0) {
    fprintf(rf, RECENT_GUI_GEOMETRY_MAIN_LOWER_PANE ": %d\n",
            recent.gui_geometry_main_lower_pane);
  }

  fprintf(rf, "\n# Packet list column pixel widths.\n");
  fprintf(rf, "# Each pair of strings consists of a column format and its pixel width.\n");
  packet_list_recent_write_all(rf);

  fprintf(rf, "\n# Open conversation dialog tabs.\n");
  fprintf(rf, "# List of conversation names, e.g. \"TCP\", \"IPv6\".\n");
  string_list = join_string_list(recent.conversation_tabs);
  fprintf(rf, RECENT_GUI_CONVERSATION_TABS ": %s\n", string_list);
  g_free(string_list);

  fprintf(rf, "\n# Open endpoint dialog tabs.\n");
  fprintf(rf, "# List of endpoint names, e.g. \"TCP\", \"IPv6\".\n");
  string_list = join_string_list(recent.endpoint_tabs);
  fprintf(rf, RECENT_GUI_ENDPOINT_TABS ": %s\n", string_list);
  g_free(string_list);

  fprintf(rf, "\n# For RLC stats, whether to use RLC PDUs found inside MAC frames.\n");
  fprintf(rf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(rf, RECENT_GUI_RLC_PDUS_FROM_MAC_FRAMES ": %s\n",
          recent.gui_rlc_use_pdus_from_mac == TRUE ? "TRUE" : "FALSE");

  if (get_last_open_dir() != NULL) {
    fprintf(rf, "\n# Last directory navigated to in File Open dialog.\n");

    if (u3_active())
      fprintf(rf, RECENT_GUI_FILEOPEN_REMEMBERED_DIR ": %s\n", u3_contract_device_path(get_last_open_dir()));
    else
      fprintf(rf, RECENT_GUI_FILEOPEN_REMEMBERED_DIR ": %s\n", get_last_open_dir());
  }

  fclose(rf);

  /
  return TRUE;
}
