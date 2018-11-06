static void dissect_mgcp_firstline(tvbuff_t *tvb, packet_info *pinfo,
				   proto_tree *tree, mgcp_info_t *mi){
  gint tvb_current_offset,tvb_previous_offset,tvb_len,tvb_current_len;
  gint tokennum, tokenlen;
  char *transid = NULL;
  char *code = NULL;
  mgcp_type_t mgcp_type = MGCP_OTHERS;
  conversation_t* conversation;
  mgcp_call_info_key mgcp_call_key;
  mgcp_call_info_key *new_mgcp_call_key = NULL;
  mgcp_call_t *mgcp_call = NULL;
  nstime_t delta;
  
  static address null_address = { AT_NONE, 0, NULL };
  proto_item* (*my_proto_tree_add_string)(proto_tree*, int, tvbuff_t*, gint,
					  gint, const char*);
  tvb_previous_offset = 0;
  tvb_len = tvb_length(tvb);
  tvb_current_len = tvb_len;
  tvb_current_offset = tvb_previous_offset;
  mi->is_duplicate = FALSE;
  mi->request_available = FALSE;

  if(tree){
    tokennum = 0;

    if(global_mgcp_dissect_tree){
      my_proto_tree_add_string = proto_tree_add_string;
    }
    else{
      my_proto_tree_add_string = proto_tree_add_string_hidden;
    }

    do {
      tvb_current_len = tvb_length_remaining(tvb,tvb_previous_offset);
      tvb_current_offset = tvb_find_guint8(tvb, tvb_previous_offset,
					   tvb_current_len, ' ');
      if(tvb_current_offset == -1){
	tvb_current_offset = tvb_len;
	tokenlen = tvb_current_len;
      }
      else{
	tokenlen = tvb_current_offset - tvb_previous_offset;
      }
      if(tokennum == 0){
        code = g_malloc(tokenlen);
        code = tvb_format_text(tvb,tvb_previous_offset,tokenlen);
        strncpy(mi->code,code,4);
        mi->code[4] = '\0';
	if(is_mgcp_verb(tvb,tvb_previous_offset,tvb_current_len)){
	  mgcp_type = MGCP_REQUEST;
	  my_proto_tree_add_string(tree,hf_mgcp_req_verb, tvb,
				   tvb_previous_offset, tokenlen,
				   code);
	}
	else if (is_mgcp_rspcode(tvb,tvb_previous_offset,tvb_current_len)){
	  mgcp_type = MGCP_RESPONSE;
	  my_proto_tree_add_string(tree,hf_mgcp_rsp_rspcode, tvb,
				   tvb_previous_offset, tokenlen,
				   code);
	}
	else {
	  break;
	}
      }
      if(tokennum == 1){
      	transid = g_malloc(tokenlen);
        transid = tvb_format_text(tvb,tvb_previous_offset,tokenlen);
        /
        mi->transid = atol(transid);
	my_proto_tree_add_string(tree,hf_mgcp_transid, tvb,
				 tvb_previous_offset, tokenlen,
				 transid);
      }
      if(tokennum == 2){
	if(mgcp_type == MGCP_REQUEST){
	  my_proto_tree_add_string(tree,hf_mgcp_req_endpoint, tvb,
				   tvb_previous_offset, tokenlen,
				   tvb_format_text(tvb, tvb_previous_offset,
						   tokenlen));
	}
	else if(mgcp_type == MGCP_RESPONSE){
	  if(tvb_current_offset < tvb_len){
	    tokenlen = tvb_find_line_end(tvb, tvb_previous_offset,
					 -1,&tvb_current_offset,FALSE);
	  }
	  else{
	    tokenlen = tvb_current_len;
	  }
	  my_proto_tree_add_string(tree, hf_mgcp_rsp_rspstring, tvb,
				   tvb_previous_offset, tokenlen,
				   tvb_format_text(tvb, tvb_previous_offset,
						   tokenlen));
	  break;
	}
      }
      if( (tokennum == 3 && mgcp_type == MGCP_REQUEST) ){
	if(tvb_current_offset < tvb_len ){
	  tokenlen = tvb_find_line_end(tvb, tvb_previous_offset,
				       -1,&tvb_current_offset,FALSE);
	}
	else{
	  tokenlen = tvb_current_len;
	}
	my_proto_tree_add_string(tree,hf_mgcp_version, tvb,
				 tvb_previous_offset, tokenlen,
				 tvb_format_text(tvb,tvb_previous_offset,
						 tokenlen));
	break;
      }
      if(tvb_current_offset < tvb_len){
	tvb_previous_offset = tvb_skip_wsp(tvb, tvb_current_offset,
					   tvb_current_len);
      }
      tokennum++;
    } while( tvb_current_offset < tvb_len && tvb_previous_offset < tvb_len
	     && tokennum <= 3);
    switch (mgcp_type){
    case MGCP_RESPONSE:
	proto_tree_add_boolean_hidden(tree, hf_mgcp_rsp, tvb, 0, 0, TRUE);
	/
	if (pinfo->ptype == PT_TCP) {
		conversation = find_conversation(&pinfo->src,
		    &pinfo->dst, pinfo->ptype, pinfo->srcport,
		    pinfo->destport, 0);
	} else {
		/
		conversation = find_conversation(&null_address,
		    &pinfo->dst, pinfo->ptype, pinfo->srcport,
		    pinfo->destport, 0);
	}
	if (conversation != NULL) {
		/
		mgcp_call_key.transid = mi->transid;
		mgcp_call_key.conversation = conversation;
		mgcp_call = g_hash_table_lookup(mgcp_calls, &mgcp_call_key);
		if(mgcp_call) {
			/
			if(mgcp_call->req_num){
				mi->request_available = TRUE;
				mgcp_call->responded = TRUE;
				proto_tree_add_uint_format(tree, hf_mgcp_req_frame,
				    tvb, 0, 0, mgcp_call->req_num,
				    "This is a response to a request in frame %u",
				    mgcp_call->req_num);
				delta.secs= pinfo->fd->abs_secs-mgcp_call->req_time.secs;
				delta.nsecs=pinfo->fd->abs_usecs*1000-mgcp_call->req_time.nsecs;
				if(delta.nsecs<0){
					delta.nsecs+=1000000000;
					delta.secs--;
				}
				proto_tree_add_time(tree, hf_mgcp_time, tvb, 0, 0,
					&delta);
			}

			if (mgcp_call->rsp_num == 0) {
				/
				mgcp_call->rsp_num = pinfo->fd->num;
			} else {
				/
				if (mgcp_call->rsp_num != pinfo->fd->num) {
					/
					mi->is_duplicate = TRUE;   
					if (check_col(pinfo->cinfo, COL_INFO)) {
						col_append_fstr(pinfo->cinfo, COL_INFO,
							", Duplicate Response %u",mi->transid);
						if (tree) {
							proto_tree_add_uint_hidden(tree,
								hf_mgcp_dup, tvb, 0,0, mi->transid);
							proto_tree_add_uint_hidden(tree,
								hf_mgcp_rsp_dup, tvb, 0,0, mi->transid);
						}
					}
				}
			}
		}
	}
      break;
    case MGCP_REQUEST:
	proto_tree_add_boolean_hidden(tree, hf_mgcp_req, tvb, 0, 0, TRUE);
	/
	if (pinfo->ptype == PT_TCP) {
		conversation = find_conversation(&pinfo->src,
		    &pinfo->dst, pinfo->ptype, pinfo->srcport,
		    pinfo->destport, 0);
	} else {
		/
		conversation = find_conversation(&pinfo->src,
		    &null_address, pinfo->ptype, pinfo->srcport,
		    pinfo->destport, 0);
	}
	if (conversation == NULL) {
		/
		if (pinfo->ptype == PT_TCP) {
			conversation = conversation_new(&pinfo->src,
			    &pinfo->dst, pinfo->ptype, pinfo->srcport,
			    pinfo->destport, 0);
		} else {
			conversation = conversation_new(&pinfo->src,
			    &null_address, pinfo->ptype, pinfo->srcport,
			    pinfo->destport, 0);
		}
	}
	
	/
	mgcp_call_key.transid = mi->transid;
	mgcp_call_key.conversation = conversation;
	
	/
	mgcp_call = g_hash_table_lookup(mgcp_calls, &mgcp_call_key);
	if (mgcp_call != NULL) {
		/
		if (pinfo->fd->num != mgcp_call->req_num) {
			/
			mi->is_duplicate = TRUE;
			if (check_col(pinfo->cinfo, COL_INFO)) {
				col_append_fstr(pinfo->cinfo, COL_INFO,
					", Duplicate Request %u",mi->transid);	
				if (tree) {
					proto_tree_add_uint_hidden(tree, 
						hf_mgcp_dup, tvb, 0,0, mi->transid);
					proto_tree_add_uint_hidden(tree, 
						hf_mgcp_req_dup, tvb, 0,0, mi->transid);
				}
			}
		}
	}
	else {
		/
		new_mgcp_call_key = g_mem_chunk_alloc(mgcp_call_info_key_chunk);
		*new_mgcp_call_key = mgcp_call_key;
		mgcp_call = g_mem_chunk_alloc(mgcp_call_info_value_chunk);
		mgcp_call->req_num = pinfo->fd->num;
		mgcp_call->rsp_num = 0;
		mgcp_call->transid = mi->transid;
		mgcp_call->responded = FALSE;
		mgcp_call->req_time.secs=pinfo->fd->abs_secs;
		mgcp_call->req_time.nsecs=pinfo->fd->abs_usecs*1000;
		strcpy(mgcp_call->code,mi->code);
		/
		g_hash_table_insert(mgcp_calls, new_mgcp_call_key, mgcp_call);
	}
	if(mgcp_call && mgcp_call->rsp_num){
		proto_tree_add_uint_format(tree, hf_mgcp_rsp_frame,
		    tvb, 0, 0, mgcp_call->rsp_num,
		    "The response to this request is in frame %u",
		    mgcp_call->rsp_num);
	}
      break;
    default:
      break;
    }
    mi->mgcp_type = mgcp_type;
    if(mgcp_call) {
    	mi->req_time.secs=mgcp_call->req_time.secs;
    	mi->req_time.nsecs=mgcp_call->req_time.nsecs;
    }
  }
  tap_queue_packet(mgcp_tap, pinfo, mi);
}
