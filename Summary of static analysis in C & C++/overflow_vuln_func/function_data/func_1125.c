static void
rpc_init_protocol(void)
{
	memset(rpc_call_table, '\0', sizeof rpc_call_table);
	rpc_call_index = 0;
	rpc_call_firstfree = 0;
}
