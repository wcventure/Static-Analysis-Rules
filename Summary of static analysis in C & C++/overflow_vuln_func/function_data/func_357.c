int
write_prefs(char **pf_path_return)
{
  char        *pf_path;
  FILE        *pf;
  GList       *clp, *col_l;
  fmt_data    *cfmt;

  /

  pf_path = get_persconffile_path(PF_NAME, TRUE);
  if ((pf = fopen(pf_path, "w")) == NULL) {
    *pf_path_return = pf_path;
    return errno;
  }

  fputs("# Configuration file for Ethereal " VERSION ".\n"
    "#\n"
    "# This file is regenerated each time preferences are saved within\n"
    "# Ethereal.  Making manual changes should be safe, however.\n", pf);

  fprintf (pf, "\n######## User Interface ########\n");
  
  fprintf(pf, "\n# Vertical scrollbars should be on right side?\n");
  fprintf(pf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(pf, PRS_GUI_SCROLLBAR_ON_RIGHT ": %s\n",
		  prefs.gui_scrollbar_on_right == TRUE ? "TRUE" : "FALSE");

  fprintf(pf, "\n# Packet-list selection bar can be used to browse w/o selecting?\n");
  fprintf(pf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(pf, PRS_GUI_PLIST_SEL_BROWSE ": %s\n",
		  prefs.gui_plist_sel_browse == TRUE ? "TRUE" : "FALSE");

  fprintf(pf, "\n# Protocol-tree selection bar can be used to browse w/o selecting?\n");
  fprintf(pf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(pf, PRS_GUI_PTREE_SEL_BROWSE ": %s\n",
		  prefs.gui_ptree_sel_browse == TRUE ? "TRUE" : "FALSE");

  fprintf(pf, "\n# Alternating colors in TreeViews?\n");
  fprintf(pf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(pf, PRS_GUI_ALTERN_COLORS ": %s\n",
		  prefs.gui_altern_colors == TRUE ? "TRUE" : "FALSE");

  fprintf(pf, "\n# Place filter toolbar inside the statusbar?\n");
  fprintf(pf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(pf, PRS_GUI_FILTER_TOOLBAR_IN_STATUSBAR ": %s\n",
                 prefs.filter_toolbar_show_in_statusbar == TRUE ? "TRUE" : "FALSE");

  fprintf(pf, "\n# Protocol-tree line style.\n");
  fprintf(pf, "# One of: NONE, SOLID, DOTTED, TABBED\n");
  fprintf(pf, PRS_GUI_PTREE_LINE_STYLE ": %s\n",
          gui_ptree_line_style_text[prefs.gui_ptree_line_style]);

  fprintf(pf, "\n# Protocol-tree expander style.\n");
  fprintf(pf, "# One of: NONE, SQUARE, TRIANGLE, CIRCULAR\n");
  fprintf(pf, PRS_GUI_PTREE_EXPANDER_STYLE ": %s\n",
		  gui_ptree_expander_style_text[prefs.gui_ptree_expander_style]);

  fprintf(pf, "\n# Hex dump highlight style.\n");
  fprintf(pf, "# One of: BOLD, INVERSE\n");
  fprintf(pf, PRS_GUI_HEX_DUMP_HIGHLIGHT_STYLE ": %s\n",
		  gui_hex_dump_highlight_style_text[prefs.gui_hex_dump_highlight_style]);

  fprintf(pf, "\n# Main Toolbar style.\n");
  fprintf(pf, "# One of: ICONS, TEXT, BOTH\n");
  fprintf(pf, PRS_GUI_TOOLBAR_MAIN_STYLE ": %s\n",
		  gui_toolbar_style_text[prefs.gui_toolbar_main_style]);

  fprintf(pf, "\n# Save window position at exit?\n");
  fprintf(pf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(pf, PRS_GUI_GEOMETRY_SAVE_POSITION ": %s\n",
		  prefs.gui_geometry_save_position == TRUE ? "TRUE" : "FALSE");

  fprintf(pf, "\n# Save window size at exit?\n");
  fprintf(pf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(pf, PRS_GUI_GEOMETRY_SAVE_SIZE ": %s\n",
		  prefs.gui_geometry_save_size == TRUE ? "TRUE" : "FALSE");
                  
  fprintf(pf, "\n# Save window maximized state at exit (GTK2 only)?\n");
  fprintf(pf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(pf, PRS_GUI_GEOMETRY_SAVE_MAXIMIZED ": %s\n",
		  prefs.gui_geometry_save_maximized == TRUE ? "TRUE" : "FALSE");
                  
  fprintf(pf, "\n# Open a console window (WIN32 only)?\n");
  fprintf(pf, "# One of: NEVER, AUTOMATIC, ALWAYS\n");
  fprintf(pf, PRS_GUI_CONSOLE_OPEN ": %s\n",
		  gui_console_open_text[prefs.gui_console_open]);

  fprintf(pf, "\n# The max. number of items in the open recent files list.\n");
  fprintf(pf, "# A decimal number.\n");
  fprintf(pf, PRS_GUI_RECENT_COUNT_MAX ": %d\n",
	          prefs.gui_recent_files_count_max);

  fprintf(pf, "\n# Where to start the File Open dialog box.\n");
  fprintf(pf, "# One of: LAST_OPENED, SPECIFIED\n");
  fprintf(pf, PRS_GUI_FILEOPEN_STYLE ": %s\n",
		  gui_fileopen_style_text[prefs.gui_fileopen_style]);

  if (prefs.gui_fileopen_dir != NULL) {
    fprintf(pf, "\n# Directory to start in when opening File Open dialog.\n");
    fprintf(pf, PRS_GUI_FILEOPEN_DIR ": %s\n",
                  prefs.gui_fileopen_dir);
  }

  fprintf(pf, "\n# The preview timeout in the File Open dialog.\n");
  fprintf(pf, "# A decimal number (in seconds).\n");
  fprintf(pf, PRS_GUI_FILEOPEN_PREVIEW ": %d\n",
	          prefs.gui_fileopen_preview);
  
  fprintf(pf, "\n# Ask to save unsaved capture files?\n");
  fprintf(pf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(pf, PRS_GUI_ASK_UNSAVED ": %s\n",
		  prefs.gui_ask_unsaved == TRUE ? "TRUE" : "FALSE");                  

  fprintf(pf, "\n# Wrap to beginning/end of file during search?\n");
  fprintf(pf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(pf, PRS_GUI_FIND_WRAP ": %s\n",
		  prefs.gui_find_wrap == TRUE ? "TRUE" : "FALSE");                  

  fprintf(pf, "\n# The path to the webbrowser.\n");
  fprintf(pf, "# Ex: mozilla %%s\n");
  fprintf(pf, PRS_GUI_WEBBROWSER ": %s\n", prefs.gui_webbrowser);

  fprintf(pf, "\n# Custom window title. (Prepended to existing titles.)\n");
  fprintf(pf, PRS_GUI_WINDOW_TITLE ": %s\n",
              prefs.gui_window_title);

  fprintf (pf, "\n######## User Interface: Layout ########\n");

  fprintf(pf, "\n# Layout type (1-6).\n");
  fprintf(pf, PRS_GUI_LAYOUT_TYPE ": %d\n",
	          prefs.gui_layout_type);

  fprintf(pf, "\n# Layout content of the panes (1-3).\n");
  fprintf(pf, "# One of: NONE, PLIST, PDETAILS, PBYTES\n");
  fprintf(pf, PRS_GUI_LAYOUT_CONTENT_1 ": %s\n",
	          gui_layout_content_text[prefs.gui_layout_content_1]);
  fprintf(pf, PRS_GUI_LAYOUT_CONTENT_2 ": %s\n",
	          gui_layout_content_text[prefs.gui_layout_content_2]);
  fprintf(pf, PRS_GUI_LAYOUT_CONTENT_3 ": %s\n",
	          gui_layout_content_text[prefs.gui_layout_content_3]);

  fprintf (pf, "\n######## User Interface: Columns ########\n");
  
  clp = prefs.col_list;
  col_l = NULL;
  while (clp) {
    cfmt = (fmt_data *) clp->data;
    col_l = g_list_append(col_l, cfmt->title);
    col_l = g_list_append(col_l, cfmt->fmt);
    clp = clp->next;
  }
  fprintf (pf, "\n# Packet list column format.\n");
  fprintf (pf, "# Each pair of strings consists of a column title and its format.\n");
  fprintf (pf, "%s: %s\n", PRS_COL_FMT, put_string_list(col_l));
  /
  g_list_free(col_l);

  fprintf (pf, "\n######## User Interface: Font ########\n");

  fprintf(pf, "\n# Font name for packet list, protocol tree, and hex dump panes (GTK version 1).\n");
  fprintf(pf, PRS_GUI_FONT_NAME_1 ": %s\n", prefs.gui_font_name1);

  fprintf(pf, "\n# Font name for packet list, protocol tree, and hex dump panes (GTK version 2).\n");
  fprintf(pf, PRS_GUI_FONT_NAME_2 ": %s\n", prefs.gui_font_name2);

  fprintf (pf, "\n######## User Interface: Colors ########\n");

  fprintf (pf, "\n# Color preferences for a marked frame.\n");
  fprintf (pf, "# Each value is a six digit hexadecimal color value in the form rrggbb.\n");
  fprintf (pf, "%s: %02x%02x%02x\n", PRS_GUI_MARKED_FG,
    (prefs.gui_marked_fg.red * 255 / 65535),
    (prefs.gui_marked_fg.green * 255 / 65535),
    (prefs.gui_marked_fg.blue * 255 / 65535));
  fprintf (pf, "%s: %02x%02x%02x\n", PRS_GUI_MARKED_BG,
    (prefs.gui_marked_bg.red * 255 / 65535),
    (prefs.gui_marked_bg.green * 255 / 65535),
    (prefs.gui_marked_bg.blue * 255 / 65535));

  fprintf (pf, "\n# TCP stream window color preferences.\n");
  fprintf (pf, "# Each value is a six digit hexadecimal color value in the form rrggbb.\n");
  fprintf (pf, "%s: %02x%02x%02x\n", PRS_STREAM_CL_FG,
    (prefs.st_client_fg.red * 255 / 65535),
    (prefs.st_client_fg.green * 255 / 65535),
    (prefs.st_client_fg.blue * 255 / 65535));
  fprintf (pf, "%s: %02x%02x%02x\n", PRS_STREAM_CL_BG,
    (prefs.st_client_bg.red * 255 / 65535),
    (prefs.st_client_bg.green * 255 / 65535),
    (prefs.st_client_bg.blue * 255 / 65535));
  fprintf (pf, "%s: %02x%02x%02x\n", PRS_STREAM_SR_FG,
    (prefs.st_server_fg.red * 255 / 65535),
    (prefs.st_server_fg.green * 255 / 65535),
    (prefs.st_server_fg.blue * 255 / 65535));
  fprintf (pf, "%s: %02x%02x%02x\n", PRS_STREAM_SR_BG,
    (prefs.st_server_bg.red * 255 / 65535),
    (prefs.st_server_bg.green * 255 / 65535),
    (prefs.st_server_bg.blue * 255 / 65535));

  fprintf(pf, "\n####### Capture ########\n");
  
  if (prefs.capture_device != NULL) {
    fprintf(pf, "\n# Default capture device\n");
    fprintf(pf, PRS_CAP_DEVICE ": %s\n", prefs.capture_device);
  }

  if (prefs.capture_devices_descr != NULL) {
    fprintf(pf, "\n# Interface descriptions.\n");
    fprintf(pf, "# Ex: eth0(eth0 descr),eth1(eth1 descr),...\n");
    fprintf(pf, PRS_CAP_DEVICES_DESCR ": %s\n", prefs.capture_devices_descr);
  }

  if (prefs.capture_devices_hide != NULL) {
    fprintf(pf, "\n# Hide interface?\n");
    fprintf(pf, "# Ex: eth0,eth3,...\n");
    fprintf(pf, PRS_CAP_DEVICES_HIDE ": %s\n", prefs.capture_devices_hide);
  }

  fprintf(pf, "\n# Capture in promiscuous mode?\n");
  fprintf(pf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(pf, PRS_CAP_PROM_MODE ": %s\n",
		  prefs.capture_prom_mode == TRUE ? "TRUE" : "FALSE");

  fprintf(pf, "\n# Update packet list in real time during capture?\n");
  fprintf(pf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(pf, PRS_CAP_REAL_TIME ": %s\n",
		  prefs.capture_real_time == TRUE ? "TRUE" : "FALSE");

  fprintf(pf, "\n# Scroll packet list during capture?\n");
  fprintf(pf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(pf, PRS_CAP_AUTO_SCROLL ": %s\n",
		  prefs.capture_auto_scroll == TRUE ? "TRUE" : "FALSE");

  fprintf(pf, "\n# Show capture info dialog while capturing?\n");
  fprintf(pf, "# TRUE or FALSE (case-insensitive).\n");
  fprintf(pf, PRS_CAP_SHOW_INFO ": %s\n",
		  prefs.capture_show_info == TRUE ? "TRUE" : "FALSE");

  fprintf (pf, "\n######## Printing ########\n");

  fprintf (pf, "\n# Can be one of \"text\" or \"postscript\".\n"
    "print.format: %s\n", pr_formats[prefs.pr_format]);

  fprintf (pf, "\n# Can be one of \"command\" or \"file\".\n"
    "print.destination: %s\n", pr_dests[prefs.pr_dest]);

  fprintf (pf, "\n# This is the file that gets written to when the "
    "destination is set to \"file\"\n"
    "%s: %s\n", PRS_PRINT_FILE, prefs.pr_file);

  fprintf (pf, "\n# Output gets piped to this command when the destination "
    "is set to \"command\"\n"
    "%s: %s\n", PRS_PRINT_CMD, prefs.pr_cmd);

  fprintf(pf, "\n####### Name Resolution ########\n");
  
  fprintf(pf, "\n# Resolve addresses to names?\n");
  fprintf(pf, "# TRUE or FALSE (case-insensitive), or a list of address types to resolve.\n");
  fprintf(pf, PRS_NAME_RESOLVE ": %s\n",
		  name_resolve_to_string(prefs.name_resolve));

  fprintf(pf, "\n# Name resolution concurrency.\n");
  fprintf(pf, "# A decimal number.\n");
  fprintf(pf, PRS_NAME_RESOLVE_CONCURRENCY ": %d\n",
		  prefs.name_resolve_concurrency);

  fprintf(pf, "\n####### Protocols ########\n");

  g_list_foreach(modules, write_module_prefs, pf);

  fclose(pf);

  /
  return 0;
}
