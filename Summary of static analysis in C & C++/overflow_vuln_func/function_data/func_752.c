static void
srt_avg_func (GtkTreeViewColumn *column _U_,
                           GtkCellRenderer   *renderer,
                           GtkTreeModel      *model,
                           GtkTreeIter       *iter,
                           gpointer           user_data)
{	
	 gchar *str;
	 guint64 td;
     gint data_column = GPOINTER_TO_INT(user_data);

     gtk_tree_model_get(model, iter, data_column, &td, -1);
     str=g_strdup_printf("%3" G_GINT64_MODIFIER "d.%05" G_GINT64_MODIFIER "d",
 		    td/100000, td%100000);
	 g_object_set(renderer, "text", str, NULL);
	 g_free(str);
}
