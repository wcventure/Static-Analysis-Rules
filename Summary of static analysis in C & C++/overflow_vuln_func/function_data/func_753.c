void
draw_srt_table_data(srt_stat_table *rst)
{
	int i;
	guint64 td;
	GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(rst->table));

	for(i=0;i<rst->num_procs;i++){
		/
		if(rst->procedures[i].stats.num==0){
			continue;
		}

		/
		/
		td=(int)rst->procedures[i].stats.tot.secs;
		td=td*100000+(int)rst->procedures[i].stats.tot.nsecs/10000;
		td/=rst->procedures[i].stats.num;
		gtk_list_store_set(store, &rst->procedures[i].iter,
				   CALLS_COLUMN,     rst->procedures[i].stats.num,
				   MIN_SRT_COLUMN,   &rst->procedures[i].stats.min,
				   MAX_SRT_COLUMN,   &rst->procedures[i].stats.max,
				   AVG_SRT_COLUMN,   td,
				   -1);
	}
}
