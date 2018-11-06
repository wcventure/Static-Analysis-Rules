static void
dissect_atp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree) {
  proto_tree *atp_tree = NULL;
  proto_item *ti;
  proto_tree *atp_info_tree;
  proto_item *info_item;
  int offset = 0;
  guint8 ctrlinfo;
  guint8 frag_number = 0;
  guint op;
  guint16 tid;
  struct aspinfo aspinfo;
  tvbuff_t   *new_tvb = NULL;
  gboolean save_fragmented;
  gboolean more_fragment = FALSE;
  int len;
  guint8 bitmap;
  
  if (check_col(pinfo->cinfo, COL_PROTOCOL))
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "ATP");

  ctrlinfo = tvb_get_guint8(tvb, offset);
  bitmap   = tvb_get_guint8(tvb, offset +1);
  tid      = tvb_get_ntohs(tvb, offset +2);

  op = ctrlinfo >> 6;

  aspinfo.reply   = (0x80 == (ctrlinfo & ATP_FUNCMASK))?1:0;
  aspinfo.release = (0xC0 == (ctrlinfo & ATP_FUNCMASK))?1:0;
  aspinfo.seq = tid;
  aspinfo.code = 0;

  /    	
  if (aspinfo.reply) {
     more_fragment = !(ATP_EOM & ctrlinfo); /
     frag_number = bitmap;
  }
  
  if (check_col(pinfo->cinfo, COL_INFO)) {
    col_clear(pinfo->cinfo, COL_INFO);
    col_add_fstr(pinfo->cinfo, COL_INFO, "%s transaction %d", 
    	val_to_str(op, atp_function_vals, "Unknown (0x%01x)"),tid);
    if (more_fragment) 
	col_append_fstr(pinfo->cinfo, COL_INFO, " [fragment]");
  }  

  if (tree) {
    ti = proto_tree_add_item(tree, proto_atp, tvb, offset, -1, FALSE);
    atp_tree = proto_item_add_subtree(ti, ett_atp);
    proto_item_set_len(atp_tree, ATP_HDRSIZE -1);

    info_item = proto_tree_add_item(atp_tree, hf_atp_ctrlinfo, tvb, offset, 1, FALSE);
    atp_info_tree = proto_item_add_subtree(info_item, ett_atp_info);

    proto_tree_add_item(atp_info_tree, hf_atp_function, tvb, offset, 1, FALSE);
    proto_tree_add_item(atp_info_tree, hf_atp_xo, tvb, offset, 1, FALSE);
    proto_tree_add_item(atp_info_tree, hf_atp_eom, tvb, offset, 1, FALSE);
    proto_tree_add_item(atp_info_tree, hf_atp_sts, tvb, offset, 1, FALSE);
    if ((ctrlinfo & (ATP_FUNCMASK|ATP_XO)) == (0x40|ATP_XO)) {
      /
      proto_tree_add_item(atp_info_tree, hf_atp_treltimer, tvb, offset, 1, FALSE);
    }

    if (!aspinfo.reply) {
      guint8 nbe = 0;
      guint8 t = bitmap;
		
      while(t) {
	nbe++;
	t >>= 1;
      }
      proto_tree_add_text(atp_tree, tvb, offset +1, 1,
			  "Bitmap: 0x%02x  %d packet(s) max", bitmap, nbe);
    }
    else {
      proto_tree_add_item(atp_tree, hf_atp_bitmap, tvb, offset +1, 1, FALSE);
    }
    proto_tree_add_item(atp_tree, hf_atp_tid, tvb, offset +2, 2, FALSE);
  }
  save_fragmented = pinfo->fragmented;

  /
  if (atp_defragment && aspinfo.reply && (more_fragment || frag_number != 0)) {
     fragment_data *fd_head;
     int hdr;
     
     hdr = ATP_HDRSIZE -1;
     if (frag_number != 0)
     	hdr += 4;	/
     len = tvb_length_remaining(tvb, hdr);
     fd_head = fragment_add_seq_check(tvb, hdr, pinfo, tid,
				     atp_fragment_table,
				     atp_reassembled_table,
				     frag_number,
				     len,
				     more_fragment);
     if (fd_head != NULL) {
	if (fd_head->next != NULL) {
            new_tvb = tvb_new_real_data(fd_head->data, fd_head->len, fd_head->len);
            tvb_set_child_real_data_tvbuff(tvb, new_tvb);
            add_new_data_source(pinfo->fd, new_tvb, "Reassembled ATP");
	    /
	    if (tree)
		    show_fragments(new_tvb, pinfo, atp_tree, fd_head);
        }
        else 
      	    new_tvb = tvb_new_subset(tvb, ATP_HDRSIZE -1, -1, -1);
     }
     else {
	new_tvb = NULL;
     }
  }
  else {
      /
     new_tvb = tvb_new_subset(tvb, ATP_HDRSIZE -1, -1,- 1);
  }
  
  if (new_tvb) {
     pinfo->private_data = &aspinfo;
     call_dissector(asp_handle, new_tvb, pinfo, tree);
  }
  else {
    /
    new_tvb = tvb_new_subset (tvb, ATP_HDRSIZE -1, -1, -1);
    call_dissector(data_handle, new_tvb, pinfo, tree);
  }
  pinfo->fragmented = save_fragmented;
  return;
}
