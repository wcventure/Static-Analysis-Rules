read_status_t
read_cap_file(capture_file *cf, int *err)
{
  gchar    *name_ptr, *load_msg, *load_fmt = " Loading: %s...";
  size_t    msg_len;
  char     *errmsg;
  char      errmsg_errno[1024+1];
  gchar     err_str[2048+1];
  int       data_offset;
  progdlg_t *progbar;
  gboolean  stop_flag;
  int       file_pos;
  float     prog_val;

  name_ptr = get_basename(cf->filename);

  msg_len = strlen(name_ptr) + strlen(load_fmt) + 2;
  load_msg = g_malloc(msg_len);
  snprintf(load_msg, msg_len, load_fmt, name_ptr);
  gtk_statusbar_push(GTK_STATUSBAR(info_bar), file_ctx, load_msg);

  /
  cf->progbar_nextstep = 0;
  /
  cf->progbar_quantum = cf->f_len/N_PROGBAR_UPDATES;

#ifndef O_BINARY
#define O_BINARY 	0
#endif

  freeze_clist(cf);

  stop_flag = FALSE;
  progbar = create_progress_dlg(load_msg, "Stop", &stop_flag);
  g_free(load_msg);

  while ((data_offset = wtap_read(cf->wth, err)) > 0) {
    /
    if (data_offset >= cf->progbar_nextstep) {
        file_pos = lseek(cf->filed, 0, SEEK_CUR);
        prog_val = (gfloat) file_pos / (gfloat) cf->f_len;
        update_progress_dlg(progbar, prog_val);
        cf->progbar_nextstep += cf->progbar_quantum;
    }

    if (stop_flag) {
      /
      destroy_progress_dlg(progbar);
      cf->state = FILE_READ_ABORTED;	/
      gtk_clist_thaw(GTK_CLIST(packet_list));	/
      close_cap_file(cf, info_bar);
      return (READ_ABORTED);
    }
    read_packet(cf, data_offset);
  }

  /
  destroy_progress_dlg(progbar);

  /
  cf->state = FILE_READ_DONE;

  /
  wtap_sequential_close(cf->wth);

  /
  cf->lnk_t = wtap_file_encap(cf->wth);

  cf->current_frame = cf->first_displayed;
  thaw_clist(cf);

  gtk_statusbar_pop(GTK_STATUSBAR(info_bar), file_ctx);
  set_display_filename(cf);

  /
  set_menus_for_capture_file(TRUE);
  set_menus_for_unsaved_capture_file(!cf->user_saved);

  /
  set_menus_for_captured_packets(TRUE);

  /
  if (cf->first_displayed != NULL)
    gtk_signal_emit_by_name(GTK_OBJECT(packet_list), "select_row", 0);

  if (data_offset < 0) {
    /
    switch (*err) {

    case WTAP_ERR_UNSUPPORTED_ENCAP:
      errmsg = "The capture file is for a network type that Ethereal doesn't support.";
      break;

    case WTAP_ERR_CANT_READ:
      errmsg = "An attempt to read from the file failed for"
               " some unknown reason.";
      break;

    case WTAP_ERR_SHORT_READ:
      errmsg = "The capture file appears to have been cut short"
               " in the middle of a packet.";
      break;

    case WTAP_ERR_BAD_RECORD:
      errmsg = "The capture file appears to be damaged or corrupt.";
      break;

    default:
      sprintf(errmsg_errno, "An error occurred while reading the"
                              " capture file: %s.", wtap_strerror(*err));
      errmsg = errmsg_errno;
      break;
    }
    snprintf(err_str, sizeof err_str, errmsg);
    simple_dialog(ESD_TYPE_CRIT, NULL, err_str);
    return (READ_ERROR);
  } else
    return (READ_SUCCESS);
}
