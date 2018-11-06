static void
desegment_ssl(tvbuff_t *tvb, packet_info *pinfo, int offset,
              guint32 seq, guint32 nxtseq,
              SslAssociation* association,
              proto_tree *root_tree, proto_tree *tree,
              SslFlow *flow)
{
    fragment_data *ipfd_head;
    gboolean must_desegment;
    gboolean called_dissector;
    int another_pdu_follows;
    int deseg_offset;
    guint32 deseg_seq;
    gint nbytes;
    proto_item *item;
    proto_item *frag_tree_item;
    proto_item *ssl_tree_item;
    struct tcp_multisegment_pdu *msp;

again:
    ipfd_head = NULL;
    must_desegment = FALSE;
    called_dissector = FALSE;
    another_pdu_follows = 0;
    msp = NULL;

    /
    pinfo->desegment_offset = 0;
    pinfo->desegment_len = 0;

    /
    deseg_offset = offset;

    /
    if ((msp = se_tree_lookup32(flow->multisegment_pdus, seq))) {
	const char* str;

	if (msp->first_frame == PINFO_FD_NUM(pinfo)) {
	    str = "";
	    col_set_str(pinfo->cinfo, COL_INFO, "[SSL segment of a reassembled PDU]");
	} else {
	    str = "Retransmitted ";
	}

	nbytes = tvb_reported_length_remaining(tvb, offset);
	proto_tree_add_text(tree, tvb, offset, nbytes,
			    "%sSSL segment data (%u byte%s)",
			    str, nbytes, plurality(nbytes, "", "s"));
	return;
    }

    /
    msp=se_tree_lookup32_le(flow->multisegment_pdus, seq-1);
    if (msp && msp->seq <= seq && msp->nxtpdu > seq) {
        int len;

        if (!PINFO_FD_VISITED(pinfo)) {
            msp->last_frame = pinfo->fd->num;
            msp->last_frame_time = pinfo->fd->abs_ts;
        }

        /
        if(msp->flags & MSP_FLAGS_REASSEMBLE_ENTIRE_SEGMENT) {
            /
            len = tvb_length_remaining(tvb, offset);
        } else {
            len = MIN(nxtseq, msp->nxtpdu) - seq;
        }

        ipfd_head = fragment_add(tvb, offset, pinfo, msp->first_frame,
                                 ssl_fragment_table, seq - msp->seq,
                                 len, (LT_SEQ (nxtseq,msp->nxtpdu)));

        if(msp->flags & MSP_FLAGS_REASSEMBLE_ENTIRE_SEGMENT) {
            msp->flags &= (~MSP_FLAGS_REASSEMBLE_ENTIRE_SEGMENT);

            /
            msp->nxtpdu = nxtseq;
        }

        if( (msp->nxtpdu < nxtseq)
        &&  (msp->nxtpdu >= seq)
        &&  (len > 0)) {
            another_pdu_follows = msp->nxtpdu - seq;
        }
    } else {
        /
        process_ssl_payload(tvb, offset, pinfo, tree, association);
        called_dissector = TRUE;

        /
        if (pinfo->desegment_len) {
            if (!PINFO_FD_VISITED(pinfo))
                must_desegment = TRUE;

            /
            deseg_offset = offset + pinfo->desegment_offset;
        }

        /
        ipfd_head = NULL;
    }


    /
    if (ipfd_head) {
        /
        if (ipfd_head->reassembled_in == pinfo->fd->num) {
            /
            tvbuff_t *next_tvb;
            int old_len;

            /
            next_tvb = tvb_new_child_real_data(tvb, ipfd_head->data,
                                               ipfd_head->datalen,
					       ipfd_head->datalen);

            /
            add_new_data_source(pinfo, next_tvb, "Reassembled SSL");

            /
            process_ssl_payload(next_tvb, 0, pinfo, tree, association);
            called_dissector = TRUE;

            /
            old_len = (int)(tvb_reported_length(next_tvb) - tvb_reported_length_remaining(tvb, offset));
            if(pinfo->desegment_len && pinfo->desegment_offset <= old_len) {
                /
                fragment_set_partial_reassembly(pinfo, msp->first_frame, ssl_fragment_table);
                /
                if (pinfo->desegment_len == DESEGMENT_ONE_MORE_SEGMENT) {
                    /
                    msp->nxtpdu = seq + tvb_reported_length_remaining(tvb, offset) + 1;
                    msp->flags |= MSP_FLAGS_REASSEMBLE_ENTIRE_SEGMENT;
                } else {
                    msp->nxtpdu = seq + tvb_reported_length_remaining(tvb, offset) + pinfo->desegment_len;
                }
                /
                another_pdu_follows = 0;
            } else {
                /
                nbytes = tvb_reported_length_remaining(tvb, offset);
                proto_tree_add_text(tree, tvb, offset, -1,
                                    "SSL segment data (%u byte%s)", nbytes,
                                    plurality(nbytes, "", "s"));

                /
                show_fragment_tree(ipfd_head, &ssl_segment_items,
                                   root_tree, pinfo, next_tvb, &frag_tree_item);
                /
                ssl_tree_item = proto_tree_get_parent(tree);
                if(frag_tree_item && ssl_tree_item) {
                    proto_tree_move_item(root_tree, ssl_tree_item, frag_tree_item);
                }

                /
                if (pinfo->desegment_len) {
                    if (!PINFO_FD_VISITED(pinfo))
                        must_desegment = TRUE;

                    /
                    deseg_offset = ipfd_head->datalen - pinfo->desegment_offset;

                    /
                    deseg_offset = tvb_reported_length(tvb) - deseg_offset;
                }
            }
        }
    }

    if (must_desegment) {
        /
        if (pinfo->desegment_len == DESEGMENT_UNTIL_FIN) {
            flow->flags |= TCP_FLOW_REASSEMBLE_UNTIL_FIN;
        }
        /
        deseg_seq = seq + (deseg_offset - offset);

        if (((nxtseq - deseg_seq) <= 1024*1024)
            &&  (!PINFO_FD_VISITED(pinfo))) {
	    if(pinfo->desegment_len == DESEGMENT_ONE_MORE_SEGMENT) {
		/
		msp = pdu_store_sequencenumber_of_next_pdu(pinfo,
		    deseg_seq, nxtseq+1, flow->multisegment_pdus);
                msp->flags |= MSP_FLAGS_REASSEMBLE_ENTIRE_SEGMENT;
            } else {
                msp = pdu_store_sequencenumber_of_next_pdu(pinfo,
		    deseg_seq, nxtseq+pinfo->desegment_len, flow->multisegment_pdus);
            }

            /
            fragment_add(tvb, deseg_offset, pinfo, msp->first_frame,
                         ssl_fragment_table, 0, nxtseq - deseg_seq,
                         LT_SEQ(nxtseq, msp->nxtpdu));
        }
    }

    if (!called_dissector || pinfo->desegment_len != 0) {
        if (ipfd_head != NULL && ipfd_head->reassembled_in != 0 &&
            !(ipfd_head->flags & FD_PARTIAL_REASSEMBLY)) {
            /
            item=proto_tree_add_uint(tree, *ssl_segment_items.hf_reassembled_in,
                                     tvb, 0, 0, ipfd_head->reassembled_in);
            PROTO_ITEM_SET_GENERATED(item);
        }

        /
        if (pinfo->desegment_offset == 0) {
            /
            col_set_str(pinfo->cinfo, COL_PROTOCOL, "SSL");
            col_set_str(pinfo->cinfo, COL_INFO, "[SSL segment of a reassembled PDU]");
        }

        /
        nbytes = tvb_reported_length_remaining(tvb, deseg_offset);
        proto_tree_add_text(tree, tvb, deseg_offset, -1,
                            "SSL segment data (%u byte%s)", nbytes,
                            plurality(nbytes, "", "s"));
    }
    pinfo->can_desegment = 0;
    pinfo->desegment_offset = 0;
    pinfo->desegment_len = 0;

    if(another_pdu_follows) {
        /
        pinfo->can_desegment=2;
        /
        col_set_fence(pinfo->cinfo, COL_INFO);
        col_set_writable(pinfo->cinfo, FALSE);
        offset += another_pdu_follows;
        seq += another_pdu_follows;
        goto again;
    }
}
