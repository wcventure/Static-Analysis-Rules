static void
desegment_tcp(tvbuff_t *tvb, packet_info *pinfo, int offset,
		guint32 seq, guint32 nxtseq,
		guint32 sport, guint32 dport,
		proto_tree *tree, proto_tree *tcp_tree)
{
	struct tcpinfo *tcpinfo = pinfo->private_data;
	fragment_data *ipfd_head=NULL;
	tcp_segment_key old_tsk, *tsk;
	gboolean must_desegment = FALSE;
	gboolean called_dissector = FALSE;
	int deseg_offset;
	guint32 deseg_seq;
	gint nbytes;

	/
	pinfo->desegment_offset = 0;
	pinfo->desegment_len = 0;

	/
	deseg_offset = offset;

	/
	old_tsk.src = &pinfo->src;
	old_tsk.dst = &pinfo->dst;
	old_tsk.sport = sport;
	old_tsk.dport = dport;
	old_tsk.seq = seq;
	tsk = g_hash_table_lookup(tcp_segment_table, &old_tsk);

	if(tsk){
		/
		ipfd_head = fragment_add(tvb, offset, pinfo, tsk->first_frame,
			tcp_fragment_table,
			seq - tsk->start_seq,
			nxtseq - seq,
			(nxtseq < (tsk->start_seq + tsk->tot_len)) );

		if(!ipfd_head){
			/
			tcp_segment_key *new_tsk;

			new_tsk = g_mem_chunk_alloc(tcp_segment_key_chunk);
			memcpy(new_tsk, tsk, sizeof(tcp_segment_key));
			new_tsk->seq=nxtseq;
			g_hash_table_insert(tcp_segment_table,new_tsk,new_tsk);
		}
	} else {
		/
		decode_tcp_ports(tvb, offset, pinfo, tree,
				sport, dport);
		called_dissector = TRUE;

		/
		if(pinfo->desegment_len) {
			if (!pinfo->fd->flags.visited)
				must_desegment = TRUE;

			/
			deseg_offset = offset + pinfo->desegment_offset;
		}

		/
		ipfd_head = NULL;
	}

	/
	if(ipfd_head){
		fragment_data *ipfd;

		/
		if(nxtseq >= (tsk->start_seq + tsk->tot_len)){
			/
			tvbuff_t *next_tvb;
			int old_len;

			/
			next_tvb = tvb_new_real_data(ipfd_head->data,
					ipfd_head->datalen, ipfd_head->datalen);

			/
			tvb_set_child_real_data_tvbuff(tvb, next_tvb);

			/
			add_new_data_source(pinfo, next_tvb, "Desegmented");

			/
			tcpinfo->seq = tsk->start_seq;

			/
			tcpinfo->is_reassembled = TRUE;

			/
			decode_tcp_ports(next_tvb, 0, pinfo, tree,
				sport, dport);
			called_dissector = TRUE;

			/
			old_len=(int)(tvb_reported_length(next_tvb)-tvb_reported_length_remaining(tvb, offset));
			if(pinfo->desegment_len &&
			    pinfo->desegment_offset<=old_len){
				tcp_segment_key *new_tsk;

				/
				fragment_set_partial_reassembly(pinfo,tsk->first_frame,tcp_fragment_table);
				tsk->tot_len = tvb_reported_length(next_tvb) + pinfo->desegment_len;

				/
				for(ipfd=ipfd_head->next; ipfd->next; ipfd=ipfd->next){
					old_tsk.seq = tsk->start_seq + ipfd->offset;
					new_tsk = g_hash_table_lookup(tcp_segment_table, &old_tsk);
					new_tsk->tot_len = tsk->tot_len;
				}

				/
				new_tsk = g_mem_chunk_alloc(tcp_segment_key_chunk);
				memcpy(new_tsk, tsk, sizeof(tcp_segment_key));
				new_tsk->seq = nxtseq;
				g_hash_table_insert(tcp_segment_table,new_tsk,new_tsk);
			} else {
				/
				nbytes =
				    tvb_reported_length_remaining(tvb, offset);
				proto_tree_add_text(tcp_tree, tvb, offset, -1,
				    "TCP segment data (%u byte%s)", nbytes,
				    plurality(nbytes, "", "s"));

				/
				show_fragment_tree(ipfd_head, &tcp_segment_items,
					tcp_tree, pinfo, next_tvb);

				/
				if(pinfo->desegment_len) {
					if (!pinfo->fd->flags.visited)
						must_desegment = TRUE;

					/
					deseg_offset =
					    ipfd_head->datalen - pinfo->desegment_offset;

					/
					deseg_offset=tvb_reported_length(tvb) - deseg_offset;
				}
			}
		}
	}

	if (must_desegment) {
	    tcp_segment_key *tsk, *new_tsk;

	    /
	    deseg_seq = seq + (deseg_offset - offset);

	    /
	    if ((nxtseq - deseg_seq) <= 1024*1024) {
		/
		tsk = g_mem_chunk_alloc(tcp_segment_key_chunk);
		tsk->src = g_mem_chunk_alloc(tcp_segment_address_chunk);
		COPY_ADDRESS(tsk->src, &pinfo->src);
		tsk->dst = g_mem_chunk_alloc(tcp_segment_address_chunk);
		COPY_ADDRESS(tsk->dst, &pinfo->dst);
		tsk->seq = deseg_seq;
		tsk->start_seq = tsk->seq;
		tsk->tot_len = nxtseq - tsk->start_seq + pinfo->desegment_len;
		tsk->first_frame = pinfo->fd->num;
		tsk->sport=sport;
		tsk->dport=dport;
		g_hash_table_insert(tcp_segment_table, tsk, tsk);

		/
		fragment_add(tvb, deseg_offset, pinfo, tsk->first_frame,
		    tcp_fragment_table,
		    tsk->seq - tsk->start_seq,
		    nxtseq - tsk->start_seq,
		    (nxtseq < tsk->start_seq + tsk->tot_len));

		/
		new_tsk = g_mem_chunk_alloc(tcp_segment_key_chunk);
		memcpy(new_tsk, tsk, sizeof(tcp_segment_key));
		new_tsk->seq = nxtseq;
		g_hash_table_insert(tcp_segment_table,new_tsk,new_tsk);
	    }
	}

	if (!called_dissector || pinfo->desegment_len != 0) {
		/
		if (pinfo->desegment_offset == 0) {
			/
			if (check_col(pinfo->cinfo, COL_PROTOCOL)){
				col_set_str(pinfo->cinfo, COL_PROTOCOL, "TCP");
			}
			if (check_col(pinfo->cinfo, COL_INFO)){
				col_set_str(pinfo->cinfo, COL_INFO, "[Desegmented TCP]");
			}
		}

		/
		nbytes = tvb_reported_length_remaining(tvb, deseg_offset);
		proto_tree_add_text(tcp_tree, tvb, deseg_offset, -1,
		    "TCP segment data (%u byte%s)", nbytes,
		    plurality(nbytes, "", "s"));
	}
	pinfo->can_desegment=0;
	pinfo->desegment_offset = 0;
	pinfo->desegment_len = 0;
}
