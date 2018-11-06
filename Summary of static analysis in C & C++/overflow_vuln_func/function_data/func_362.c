static void
dissect_mgcp_message(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
		     proto_tree *mgcp_tree, proto_tree *ti){

  /
  gint sectionlen;
  gint tvb_sectionend,tvb_sectionbegin, tvb_len, tvb_current_len;
  tvbuff_t *next_tvb;
  static mgcp_info_t mi;
    
  /
  tvb_sectionend = 0;
  tvb_sectionbegin = tvb_sectionend;
  sectionlen = 0;
  tvb_len = tvb_length(tvb);
  tvb_current_len  = tvb_len;

  /
  if(is_mgcp_verb(tvb,0,tvb_len) || is_mgcp_rspcode(tvb,0,tvb_len)){

    /
    if (tree && mgcp_tree) {

      /
      tvb_sectionbegin = 0;
      tvb_current_len = tvb_len;
      tvb_sectionend = tvb_sectionbegin;
      sectionlen = tvb_find_line_end(tvb,0,-1,&tvb_sectionend,FALSE);
      if( sectionlen > 0){
	dissect_mgcp_firstline(tvb_new_subset(tvb, tvb_sectionbegin,
					      sectionlen,-1), pinfo,
			       mgcp_tree, &mi);
      }
      tvb_sectionbegin = tvb_sectionend;

      /
      if(tvb_sectionbegin < tvb_len){
	sectionlen = tvb_find_null_line(tvb, tvb_sectionbegin, -1,
					&tvb_sectionend);
	dissect_mgcp_params(tvb_new_subset(tvb, tvb_sectionbegin,
					   sectionlen, -1),
			    mgcp_tree);
	tvb_sectionbegin = tvb_sectionend;
      }

      /
      sectionlen = tvb_sectionend;
      proto_item_set_len(ti,sectionlen);

      /

      /
      if(global_mgcp_raw_text){
	mgcp_raw_text_add(tvb_new_subset(tvb,0,tvb_len,-1),
			  mgcp_tree);
      }

      /
      if( tvb_sectionend < tvb_len && global_mgcp_dissect_tree == TRUE){
	next_tvb = tvb_new_subset(tvb, tvb_sectionend, -1, -1);
	call_dissector(sdp_handle, next_tvb, pinfo, tree);
      }
    }
  }
}
