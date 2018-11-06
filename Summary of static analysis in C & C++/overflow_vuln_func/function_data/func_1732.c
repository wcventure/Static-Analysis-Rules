static void msg_stats_tree_init(stats_tree* st) {
	st_node_msg = stats_tree_create_node(st, "Messages by Type", 0, TRUE);	
	st_node_dir = stats_tree_create_node(st, "Messages by Direction", 0, TRUE);	
}
