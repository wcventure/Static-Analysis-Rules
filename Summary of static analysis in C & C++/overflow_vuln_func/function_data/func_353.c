static guint32 dissect_mux_pdu_fragment( tvbuff_t *tvb, guint32 start_offset, packet_info * pinfo,
					 guint32* pkt_offset,
					 proto_tree *tree,
					 proto_tree **h223_tree_p,
					 h223_call_info *call_info,
					 gboolean *pdu_found)
{
    proto_item *h223_item = NULL;
    proto_tree *volatile h223_tree = *h223_tree_p;
    tvbuff_t *volatile next_tvb;
    volatile guint32 offset = start_offset;
    gboolean more_frags = TRUE;
	proto_tree *pdu_tree;

    h223_call_direction_data *dirdata = &call_info -> direction_data[pinfo->p2p_dir];

    dirdata -> current_pdu_read = 0;
    dirdata -> current_pdu_minlen = 0;
    dirdata -> current_pdu_header_parsed = FALSE;

    while( more_frags && offset < tvb_reported_length( tvb )) {
        guint8 byte = tvb_get_guint8(tvb, offset++);
        dirdata -> current_pdu_read++;
        
        /
        if(dirdata -> current_pdu_read <= 4) {
            dirdata -> header_buf <<= 8;
            dirdata -> header_buf |= byte;
        }

        /
        dirdata -> tail_buf <<= 8;
        dirdata -> tail_buf |= byte;

        /
        if(!dirdata -> current_pdu_header_parsed)
            /
            (attempt_mux_header_parse[call_info->h223_level])(dirdata);

        if(dirdata -> current_pdu_read >= dirdata -> current_pdu_minlen) {
            if(h223_mux_check_hdlc(call_info->h223_level,dirdata)) {
                dirdata -> current_pdu_minlen = 0;
                dirdata -> current_pdu_read = 0;
                dirdata -> current_pdu_header_parsed = FALSE;
                more_frags = FALSE;
            }
        }
    }

    if( more_frags ) {
        /
        pinfo->desegment_offset = offset - dirdata->current_pdu_read;
        if(dirdata->current_pdu_read > dirdata->current_pdu_minlen)
            pinfo->desegment_len = 1;
        else
            pinfo->desegment_len = dirdata->current_pdu_minlen - dirdata->current_pdu_read;
        return offset;
    }

    if(!*h223_tree_p) {
        /
        if (tree) {
            h223_item = proto_tree_add_item (tree, proto_h223, tvb, 0, -1, FALSE);
            h223_tree = proto_item_add_subtree (h223_item, ett_h223);
            *h223_tree_p = h223_tree;
        }
    }
    *pdu_found = TRUE;

    /
    next_tvb = tvb_new_subset(tvb, start_offset, offset-start_offset,
			      offset-start_offset);


    *pkt_offset += tvb_reported_length( next_tvb );

    /
    if( dirdata->first_pdu ) {
        dirdata->first_pdu = FALSE;
        pdu_tree = NULL;
        if( h223_tree ) {
            proto_item *pdu_item = proto_tree_add_item (h223_tree, hf_h223_non_h223_data, tvb, 0, -1, FALSE);
            pdu_tree = proto_item_add_subtree (pdu_item, ett_h223_non_h223_data);
        }
        call_dissector(data_handle,tvb, pinfo, pdu_tree);
        return offset;
    }

    /
    pdu_offset = *pkt_offset - tvb_reported_length( next_tvb );
    TRY {
        dissect_mux_pdu( next_tvb, pinfo, *pkt_offset - tvb_reported_length( next_tvb ), h223_tree, call_info);
    }

    CATCH2(BoundsError,ReportedBoundsError) {
        if (check_col(pinfo->cinfo, COL_INFO))
            col_append_str(pinfo->cinfo, COL_INFO,
                           "[Malformed Packet]");
        proto_tree_add_protocol_format(h223_tree, proto_malformed,
                                       tvb, 0, 0, "[Malformed Packet: %s]", pinfo->current_proto);
    }

    ENDTRY;

    return offset;
}
