static void
srt_time_func (GtkTreeViewColumn *column _U_,
                           GtkCellRenderer   *renderer,
                           GtkTreeModel      *model,
                           GtkTreeIter       *iter,
                           gpointer           user_data)
{	
	 gchar *str;
	 nstime_t *data;

     /
     gint data_column = GPOINTER_TO_INT(user_data);

     gtk_tree_model_get(model, iter, data_column, &data, -1);
     if (!data) {
	     g_object_set(renderer, "text", "", NULL);
	     return;
	 }
	 str = g_strdup_printf("%3d.%05d", (int)data->secs, data->nsecs/10000);
	 g_object_set(renderer, "text", str, NULL);
	 g_free(str);
}
