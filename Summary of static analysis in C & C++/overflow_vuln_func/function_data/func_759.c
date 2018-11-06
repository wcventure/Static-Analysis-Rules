static int
add_packet_to_packet_list(frame_data *fdata, capture_file *cf,
	dfilter_t *dfcode,
	union wtap_pseudo_header *pseudo_header, const guchar *buf,
	gboolean refilter)
{
  gint          row;
  gboolean	create_proto_tree = FALSE;
  epan_dissect_t *edt;

  /
  fdata->cum_bytes  = cum_bytes + fdata->pkt_len;

  /
  if (nstime_is_unset(&first_ts)) {
    first_ts  = fdata->abs_ts;
  }
  /
  if(fdata->flags.ref_time){
    first_ts = fdata->abs_ts;
  }

  /
  if (nstime_is_unset(&prev_dis_ts)) {
    prev_dis_ts = fdata->abs_ts;
  }

  /
  nstime_delta(&fdata->rel_ts, &fdata->abs_ts, &first_ts);

  /
  if ((gint32)cf->elapsed_time.secs < fdata->rel_ts.secs
  || ((gint32)cf->elapsed_time.secs == fdata->rel_ts.secs && (gint32)cf->elapsed_time.nsecs < fdata->rel_ts.nsecs)) {
    cf->elapsed_time = fdata->rel_ts;
  }

  /
  nstime_delta(&fdata->del_dis_ts, &fdata->abs_ts, &prev_dis_ts);

  /
  if ((dfcode != NULL && refilter) || color_filters_used()
      || num_tap_filters != 0 || have_custom_cols(&cf->cinfo))
	  create_proto_tree = TRUE;

  /
  edt = epan_dissect_new(create_proto_tree, FALSE);

  if (dfcode != NULL && refilter) {
      epan_dissect_prime_dfilter(edt, dfcode);
  }
  /
  if (color_filters_used()) {
      color_filters_prime_edt(edt);
  }

  col_custom_prime_edt(edt, &cf->cinfo);

  tap_queue_init(edt);
  epan_dissect_run(edt, pseudo_header, buf, fdata, &cf->cinfo);
  tap_push_tapped_queue(edt);

  /
  if (dfcode != NULL) {
    if (refilter) {
      fdata->flags.passed_dfilter = dfilter_apply_edt(dfcode, edt) ? 1 : 0;
    }
  } else
    fdata->flags.passed_dfilter = 1;

  if( (fdata->flags.passed_dfilter)
   || (edt->pi.fd->flags.ref_time) ){
    /
    if(edt->pi.fd->flags.ref_time){
      /
      cum_bytes = fdata->pkt_len;
      fdata->cum_bytes =  cum_bytes;
    } else {
      /
      cum_bytes += fdata->pkt_len;
    }

    epan_dissect_fill_in_columns(edt);

    /
    if (cf->first_displayed == NULL)
      cf->first_displayed = fdata;

    /
    cf->last_displayed = fdata;

    /
#if (GLIB_MAJOR_VERSION >= 2)
    fdata->col_expr.col_expr = g_strdupv(cf->cinfo.col_expr.col_expr);
    fdata->col_expr.col_expr_val = g_strdupv(cf->cinfo.col_expr.col_expr_val);
#else
    {
      gint i;

      fdata->col_expr.col_expr = (gchar **) g_malloc(sizeof(gchar *) * (cf->cinfo.num_cols + 1));
      fdata->col_expr.col_expr_val = (gchar **) g_malloc(sizeof(gchar *) * (cf->cinfo.num_cols + 1));

      for (i=0; i <= cf->cinfo.num_cols; i++)
      {
        fdata->col_expr.col_expr[i] = g_strdup(cf->cinfo.col_expr.col_expr[i]);
        fdata->col_expr.col_expr_val[i] = g_strdup(cf->cinfo.col_expr.col_expr_val[i]);
      }
      fdata->col_expr.col_expr[i] = NULL;
      fdata->col_expr.col_expr_val[i] = NULL;
    }
#endif
    row = packet_list_append(cf->cinfo.col_data, fdata);

    /
      fdata->color_filter = color_filters_colorize_packet(row, edt);
      if (fdata->flags.marked) {
          packet_list_set_colors(row, &prefs.gui_marked_fg, &prefs.gui_marked_bg);
      }

    /
    prev_dis_ts = fdata->abs_ts;

    cf->displayed_count++;
  } else {
    /
    row = -1;
  }
  epan_dissect_free(edt);
  return row;
}
