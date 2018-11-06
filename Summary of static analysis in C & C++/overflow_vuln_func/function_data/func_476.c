static int
dissect_a11( tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
  /
  proto_item	*ti;
  proto_tree	*a11_tree=NULL;
  proto_item    *tf;
  proto_tree    *flags_tree;
  guint8         type;
  guint8         flags;
  size_t         offset=0;
  const guint8  *reftime;
  
  if (!tvb_bytes_exist(tvb, offset, 1))
	return 0;	/

  type = tvb_get_guint8(tvb, offset);
  if (match_strval(type, a11_types) == NULL)
	return 0;	/

  /
  
  if (check_col(pinfo->cinfo, COL_PROTOCOL)) 
	col_set_str(pinfo->cinfo, COL_PROTOCOL, "3GPP2 A11");
  if (check_col(pinfo->cinfo, COL_INFO)) 
	col_clear(pinfo->cinfo, COL_INFO);

  switch (type) {
  case REGISTRATION_REQUEST:
	if (check_col(pinfo->cinfo, COL_INFO)) 
	  col_add_fstr(pinfo->cinfo, COL_INFO, "Reg Request: PDSN=%s PCF=%s", 
				   ip_to_str(tvb_get_ptr(tvb, 8, 4)),
				   ip_to_str(tvb_get_ptr(tvb,12,4)));
	
	if (tree) {
	  ti = proto_tree_add_item(tree, proto_a11, tvb, offset, -1, FALSE);
	  a11_tree = proto_item_add_subtree(ti, ett_a11);
	  
	  /
	  proto_tree_add_uint(a11_tree, hf_a11_type, tvb, offset, 1, type);
	  offset++;
	  
	  /
	  flags = tvb_get_guint8(tvb, offset);
	  tf = proto_tree_add_uint(a11_tree, hf_a11_flags, tvb,
							   offset, 1, flags);
	  flags_tree = proto_item_add_subtree(tf, ett_a11_flags);
	  proto_tree_add_boolean(flags_tree, hf_a11_s, tvb, offset, 1, flags);
	  proto_tree_add_boolean(flags_tree, hf_a11_b, tvb, offset, 1, flags);
	  proto_tree_add_boolean(flags_tree, hf_a11_d, tvb, offset, 1, flags);
	  proto_tree_add_boolean(flags_tree, hf_a11_m, tvb, offset, 1, flags);
	  proto_tree_add_boolean(flags_tree, hf_a11_g, tvb, offset, 1, flags);
	  proto_tree_add_boolean(flags_tree, hf_a11_v, tvb, offset, 1, flags);
	  proto_tree_add_boolean(flags_tree, hf_a11_t, tvb, offset, 1, flags);
	  offset++;

	  /
	  proto_tree_add_item(a11_tree, hf_a11_life, tvb, offset, 2, FALSE);
	  offset +=2;
	  
	  /
	  proto_tree_add_item(a11_tree, hf_a11_homeaddr, tvb, offset, 4, FALSE);
	  offset += 4;
	  
	  /
	  proto_tree_add_item(a11_tree, hf_a11_haaddr, tvb, offset, 4, FALSE);
	  offset += 4;
	  
	  /
	  proto_tree_add_item(a11_tree, hf_a11_coa, tvb, offset, 4, FALSE);
	  offset += 4;

	  /
	  reftime = tvb_get_ptr(tvb, offset, 8);
	  proto_tree_add_bytes_format_value(a11_tree, hf_a11_ident, tvb,
					    offset, 8, reftime,
					    "%s",
					    ntp_fmt_ts(reftime));
	  offset += 8;
		
	} /
	break;
  case REGISTRATION_REPLY:
	if (check_col(pinfo->cinfo, COL_INFO)) 
	  col_add_fstr(pinfo->cinfo, COL_INFO, "Reg Reply:   PDSN=%s, Code=%u", 
				   ip_to_str(tvb_get_ptr(tvb,8,4)), tvb_get_guint8(tvb,1));
	
	if (tree) {
	  /
	  ti = proto_tree_add_item(tree, proto_a11, tvb, offset, -1, FALSE);
	  a11_tree = proto_item_add_subtree(ti, ett_a11);
	  
	  /
  	  proto_tree_add_uint(a11_tree, hf_a11_type, tvb, offset, 1, type);
	  offset++;
	  
	  /
	  proto_tree_add_item(a11_tree, hf_a11_code, tvb, offset, 1, FALSE);
	  offset++;

	  /
	  proto_tree_add_item(a11_tree, hf_a11_life, tvb, offset, 2, FALSE);
	  offset += 2;

	  /
	  proto_tree_add_item(a11_tree, hf_a11_homeaddr, tvb, offset, 4, FALSE);
	  offset += 4;

	  /
	  proto_tree_add_item(a11_tree, hf_a11_haaddr, tvb, offset, 4, FALSE);
	  offset += 4;

	  /
	  reftime = tvb_get_ptr(tvb, offset, 8);
	  proto_tree_add_bytes_format_value(a11_tree, hf_a11_ident, tvb,
					    offset, 8,
					    reftime,
					    "%s",
					    ntp_fmt_ts(reftime));
	  offset += 8;
	} /
	
	break;
  case REGISTRATION_UPDATE:
	if (check_col(pinfo->cinfo, COL_INFO)) 
	  col_add_fstr(pinfo->cinfo, COL_INFO,"Reg Update:  PDSN=%s", 
				   ip_to_str(tvb_get_ptr(tvb,8,4)));
	if (tree) {
	  /
	  ti = proto_tree_add_item(tree, proto_a11, tvb, offset, -1, FALSE);
	  a11_tree = proto_item_add_subtree(ti, ett_a11);
	  
	  /
  	  proto_tree_add_uint(a11_tree, hf_a11_type, tvb, offset, 1, type);
	  offset++;

	  /
	  offset+=3;
	
	  /
	  proto_tree_add_item(a11_tree, hf_a11_homeaddr, tvb, offset, 4, FALSE);
	  offset += 4;

	  /
	  proto_tree_add_item(a11_tree, hf_a11_haaddr, tvb, offset, 4, FALSE);
	  offset += 4;

	  /
	  reftime = tvb_get_ptr(tvb, offset, 8);
	  proto_tree_add_bytes_format_value(a11_tree, hf_a11_ident, tvb,
					    offset, 8,
					    reftime,
					    "%s",
					    ntp_fmt_ts(reftime));
	  offset += 8;

	} /
	break;
  case REGISTRATION_ACK:
	if (check_col(pinfo->cinfo, COL_INFO)) 
	  col_add_fstr(pinfo->cinfo, COL_INFO, "Reg Ack:     PCF=%s Status=%u", 
				   ip_to_str(tvb_get_ptr(tvb, 8, 4)),
				   tvb_get_guint8(tvb,3));
	if (tree) {
	  /
	  ti = proto_tree_add_item(tree, proto_a11, tvb, offset, -1, FALSE);
	  a11_tree = proto_item_add_subtree(ti, ett_a11);
	  
	  /
  	  proto_tree_add_uint(a11_tree, hf_a11_type, tvb, offset, 1, type);
	  offset++;
	
	  /
	  offset+=2;

	  /
	  proto_tree_add_item(a11_tree, hf_a11_status, tvb, offset, 1, FALSE);
	  offset++;

	  /
	  proto_tree_add_item(a11_tree, hf_a11_homeaddr, tvb, offset, 4, FALSE);
	  offset += 4;

	  /
	  proto_tree_add_item(a11_tree, hf_a11_coa, tvb, offset, 4, FALSE);
	  offset += 4;

	  /
	  reftime = tvb_get_ptr(tvb, offset, 8);
	  proto_tree_add_bytes_format_value(a11_tree, hf_a11_ident, tvb,
					    offset, 8,
					    reftime,
					    "%s",
					    ntp_fmt_ts(reftime));
	  offset += 8;

	} /
	break;
  case SESSION_UPDATE: /
	if (check_col(pinfo->cinfo, COL_INFO)) 
	  col_add_fstr(pinfo->cinfo, COL_INFO,"Ses Update:  PDSN=%s", 
				   ip_to_str(tvb_get_ptr(tvb,8,4)));
	if (tree) {
	  /
	  ti = proto_tree_add_item(tree, proto_a11, tvb, offset, -1, FALSE);
	  a11_tree = proto_item_add_subtree(ti, ett_a11);
	  
	  /
  	  proto_tree_add_uint(a11_tree, hf_a11_type, tvb, offset, 1, type);
	  offset++;

	  /
	  offset+=3;
	
	  /
	  proto_tree_add_item(a11_tree, hf_a11_homeaddr, tvb, offset, 4, FALSE);
	  offset += 4;

	  /
	  proto_tree_add_item(a11_tree, hf_a11_haaddr, tvb, offset, 4, FALSE);
	  offset += 4;

	  /
	  reftime = tvb_get_ptr(tvb, offset, 8);
	  proto_tree_add_bytes_format_value(a11_tree, hf_a11_ident, tvb,
					    offset, 8,
					    reftime,
					    "%s",
					    ntp_fmt_ts(reftime));
	  offset += 8;

	} /
	break;
  case SESSION_ACK: /
	if (check_col(pinfo->cinfo, COL_INFO)) 
	  col_add_fstr(pinfo->cinfo, COL_INFO, "Ses Upd Ack: PCF=%s, Status=%u", 
				   ip_to_str(tvb_get_ptr(tvb, 8, 4)),
				   tvb_get_guint8(tvb,3));
	if (tree) {
	  /
	  ti = proto_tree_add_item(tree, proto_a11, tvb, offset, -1, FALSE);
	  a11_tree = proto_item_add_subtree(ti, ett_a11);
	  
	  /
  	  proto_tree_add_uint(a11_tree, hf_a11_type, tvb, offset, 1, type);
	  offset++;
	
	  /
	  offset+=2;

	  /
	  proto_tree_add_item(a11_tree, hf_a11_status, tvb, offset, 1, FALSE);
	  offset++;

	  /
	  proto_tree_add_item(a11_tree, hf_a11_homeaddr, tvb, offset, 4, FALSE);
	  offset += 4;

	  /
	  proto_tree_add_item(a11_tree, hf_a11_coa, tvb, offset, 4, FALSE);
	  offset += 4;

	  /
	  reftime = tvb_get_ptr(tvb, offset, 8);
	  proto_tree_add_bytes_format_value(a11_tree, hf_a11_ident, tvb,
					    offset, 8,
					    reftime,
					    "%s",
					    ntp_fmt_ts(reftime));
	  offset += 8;

	} /
	break;
  } /

  if (tree) {
	if (tvb_reported_length_remaining(tvb, offset) > 0)
	  dissect_a11_extensions(tvb, offset, a11_tree);
  }
  return tvb_length(tvb);
} /
