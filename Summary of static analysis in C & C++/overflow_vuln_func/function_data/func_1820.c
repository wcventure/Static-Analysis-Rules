static ndps_req_hash_value*
ndps_hash_insert(conversation_t *conversation, guint32 ndps_xport)
{
	ndps_req_hash_key		*request_key;
	ndps_req_hash_value		*request_value;

	/
	request_key = se_alloc(sizeof(ndps_req_hash_key));
	request_key->conversation = conversation;
	request_key->ndps_xport = ndps_xport;

	request_value = se_alloc(sizeof(ndps_req_hash_value));
	request_value->ndps_prog = 0;
	request_value->ndps_func = 0;
	request_value->ndps_frame_num = 0;
    request_value->ndps_frag = FALSE;
    request_value->ndps_end_frag = 0;
       
	g_hash_table_insert(ndps_req_hash, request_key, request_value);

	return request_value;
}
