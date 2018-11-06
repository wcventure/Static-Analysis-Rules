static void
mgcp_init_protocol(void)
{
	if (mgcp_calls != NULL) {
		g_hash_table_destroy(mgcp_calls);
		mgcp_calls = NULL;
	}
	if (mgcp_call_info_key_chunk != NULL) {
		g_mem_chunk_destroy(mgcp_call_info_key_chunk);
		mgcp_call_info_key_chunk = NULL;
	}
	if (mgcp_call_info_value_chunk != NULL) {
		g_mem_chunk_destroy(mgcp_call_info_value_chunk);
		mgcp_call_info_value_chunk = NULL;
	}

	mgcp_calls = g_hash_table_new(mgcp_call_hash, mgcp_call_equal);
	mgcp_call_info_key_chunk = g_mem_chunk_new("call_info_key_chunk",
	    sizeof(mgcp_call_info_key),
	    200 * sizeof(mgcp_call_info_key),
	    G_ALLOC_ONLY);
	mgcp_call_info_value_chunk = g_mem_chunk_new("call_info_value_chunk",
	    sizeof(mgcp_call_t),
	    200 * sizeof(mgcp_call_t),
	    G_ALLOC_ONLY);	
}
