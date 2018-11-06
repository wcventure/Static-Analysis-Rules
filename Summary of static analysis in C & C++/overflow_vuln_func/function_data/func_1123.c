static rpc_call_info*
rpc_call_lookup(rpc_call_info *call)
{
	int i;

	i = rpc_call_index;
	do {
		if (
			rpc_call_table[i].xid == call->xid &&
			rpc_call_table[i].conversation == call->conversation
		) {
			return &rpc_call_table[i];
		}
		if (rpc_call_firstfree) {
			/
			i = (i-1+rpc_call_firstfree) % rpc_call_firstfree;
		}
	} while (i!=rpc_call_index);
	return NULL;
}
