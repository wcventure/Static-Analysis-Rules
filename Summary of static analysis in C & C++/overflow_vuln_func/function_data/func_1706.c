static gchar *
get_filter_from_packet_list_row_and_column(gpointer data)
{
    gint    row = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(data), E_MPACKET_LIST_ROW_KEY));
#ifdef NEW_PACKET_LIST
    gint    column = new_packet_list_get_column_id (GPOINTER_TO_INT(g_object_get_data(G_OBJECT(data), E_MPACKET_LIST_COL_KEY)));
#else
    gint    column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(data), E_MPACKET_LIST_COL_KEY));
#endif
    frame_data *fdata;
    gchar      *buf=NULL;

#ifdef NEW_PACKET_LIST
    fdata = (frame_data *) new_packet_list_get_row_data(row);
#else
    fdata = (frame_data *) packet_list_get_row_data(row);
#endif

    if (fdata != NULL) {
        epan_dissect_t edt;

        if (!cf_read_frame(&cfile, fdata))
            return NULL; /
        /
        epan_dissect_init(&edt, have_custom_cols(&cfile.cinfo), FALSE);
        col_custom_prime_edt(&edt, &cfile.cinfo);

        epan_dissect_run(&edt, &cfile.pseudo_header, cfile.pd, fdata,
                 &cfile.cinfo);
        epan_dissect_fill_in_columns(&edt, TRUE, TRUE);

        if (strlen(cfile.cinfo.col_expr.col_expr[column]) != 0 &&
            strlen(cfile.cinfo.col_expr.col_expr_val[column]) != 0) {
            /
            if (cfile.cinfo.col_fmt[column] == COL_CUSTOM) {
                header_field_info *hfi = proto_registrar_get_byname(cfile.cinfo.col_custom_field[column]);
                if (hfi->parent == -1) {
                    /
                    buf = se_strdup(cfile.cinfo.col_expr.col_expr[column]);
                } else if (hfi->type == FT_STRING) {
                    /
                    buf = se_strdup_printf("%s == \"%s\"", cfile.cinfo.col_expr.col_expr[column],
                                           cfile.cinfo.col_expr.col_expr_val[column]);
                }
            }
	    if (buf == NULL) {
                buf = se_strdup_printf("%s == %s", cfile.cinfo.col_expr.col_expr[column],
                                       cfile.cinfo.col_expr.col_expr_val[column]);
            }
        }

        epan_dissect_cleanup(&edt);
    }

    return buf;
}
