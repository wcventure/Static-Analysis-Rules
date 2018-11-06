static void add_to_clist(GtkCList *clist, guint32 number, guint16 seq_num,
                         double delta, double jitter, double bandwidth, gchar *status, gboolean marker,
                         gchar *timeStr, guint32 pkt_len, GdkColor *bg_color)
{
	guint added_row;
	gchar *data[9];
	gchar field[9][32];
	char *savelocale;
	GdkColor fg_color = COLOR_FOREGROUND;

	data[0]=&field[0][0];
	data[1]=&field[1][0];
	data[2]=&field[2][0];
	data[3]=&field[3][0];
	data[4]=&field[4][0];
	data[5]=&field[5][0];
	data[6]=&field[6][0];
	data[7]=&field[7][0];
	data[8]=&field[8][0];

	/
	savelocale = setlocale(LC_NUMERIC, NULL);
	/
	setlocale(LC_NUMERIC, "C");
	g_snprintf(field[0], 20, "%u", number);
	g_snprintf(field[1], 20, "%u", seq_num);
	g_snprintf(field[2], 20, "%.2f", delta);
	g_snprintf(field[3], 20, "%.2f", jitter);
	g_snprintf(field[4], 20, "%.2f", bandwidth);
	g_snprintf(field[5], 20, "%s", marker? "SET" : "");
	g_snprintf(field[6], 40, "%s", status);
	g_snprintf(field[7], 32, "%s", timeStr);
	g_snprintf(field[8], 20, "%u", pkt_len);
	/
	setlocale(LC_NUMERIC, savelocale);

	added_row = gtk_clist_append(GTK_CLIST(clist), data);
	gtk_clist_set_row_data(GTK_CLIST(clist), added_row, GUINT_TO_POINTER(number));
	gtk_clist_set_background(GTK_CLIST(clist), added_row, bg_color);
	gtk_clist_set_foreground(GTK_CLIST(clist), added_row, &fg_color);
}
