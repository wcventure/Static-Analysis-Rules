static void
rpc_call_insert(rpc_call_info *call)
{
	/
	if (rpc_call_firstfree<RPC_CALL_TABLE_LENGTH) {
		/
		/
		rpc_call_index = rpc_call_firstfree;
		/
		rpc_call_firstfree++;
		/
	}
	else {
		/
		/
		rpc_call_index = (rpc_call_index+1) % rpc_call_firstfree;
	}
		
	/
	memcpy(&rpc_call_table[rpc_call_index],call,sizeof(*call));
	return;
}
