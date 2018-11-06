static void
dissect_asp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree) 
{
  struct aspinfo *aspinfo = pinfo->private_data;
  int offset = 0;
  proto_tree *asp_tree = NULL;
  proto_item *ti;
  guint8 fn;
  gint32 error;
  int len;
  conversation_t	*conversation;
  asp_request_key request_key, *new_request_key;
  asp_request_val *request_val;
    
  if (check_col(pinfo->cinfo, COL_PROTOCOL))
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "ASP");
  if (check_col(pinfo->cinfo, COL_INFO))
    col_clear(pinfo->cinfo, COL_INFO);

  conversation = find_conversation(&pinfo->src, &pinfo->dst, pinfo->ptype,
		pinfo->srcport, pinfo->destport, 0);

  if (conversation == NULL)
  {
	conversation = conversation_new(&pinfo->src, &pinfo->dst,
			pinfo->ptype, pinfo->srcport, pinfo->destport, 0);
  }

  request_key.conversation = conversation->index;	
  request_key.seq = aspinfo->seq;

  request_val = (asp_request_val *) g_hash_table_lookup(
								asp_request_hash, &request_key);

  if (!request_val && !aspinfo->reply && !aspinfo->release)  {
	 fn = tvb_get_guint8(tvb, offset);
	 new_request_key = g_mem_chunk_alloc(asp_request_keys);
	 *new_request_key = request_key;

	 request_val = g_mem_chunk_alloc(asp_request_vals);
	 request_val->command = fn;

	 g_hash_table_insert(asp_request_hash, new_request_key,
								request_val);
  }

  if (!request_val) { 
	return;
  }

  fn = request_val->command;

  if (check_col(pinfo->cinfo, COL_INFO)) {
	if (aspinfo->reply)
		col_add_fstr(pinfo->cinfo, COL_INFO, "Reply tid %d",aspinfo->seq);
	else if (aspinfo->release)
		col_add_fstr(pinfo->cinfo, COL_INFO, "Release tid %d",aspinfo->seq);
	else
		col_add_fstr(pinfo->cinfo, COL_INFO, "Function: %s  tid %d",
      				val_to_str(fn, asp_func_vals, "Unknown (0x%01x)"), aspinfo->seq);
  }

  if (tree) {
    ti = proto_tree_add_item(tree, proto_asp, tvb, offset, -1, FALSE);
    asp_tree = proto_item_add_subtree(ti, ett_asp);
    if (!aspinfo->reply && !aspinfo->release) {
      proto_tree_add_item(asp_tree, hf_asp_func, tvb, offset, 1, FALSE);
    }
    else { /
      error = tvb_get_ntohl(tvb, offset);
      if (error <= 0) 
	proto_tree_add_item(asp_tree, hf_asp_error, tvb, offset, 4, FALSE);
    }
  }
  aspinfo->command = fn;
  offset += 4;
  len = tvb_reported_length_remaining(tvb,offset);
  if (!aspinfo->release &&
  		   (fn == ASPFUNC_CMD || fn  == ASPFUNC_WRITE)) {
	tvbuff_t   *new_tvb;

	if (asp_tree)
		proto_item_set_len(asp_tree, 4);
	new_tvb = tvb_new_subset(tvb, offset,-1,len);
	call_dissector(afp_handle, new_tvb, pinfo, tree);  	
  }
  else {	
	call_dissector(data_handle,tvb_new_subset(tvb, offset,-1,len), pinfo, tree); 
  }
}
