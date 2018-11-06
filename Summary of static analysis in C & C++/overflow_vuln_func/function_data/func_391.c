static void aeron_frame_stream_analysis_setup(packet_info * pinfo, aeron_packet_info_t * info, aeron_frame_info_t * finfo, aeron_stream_t * stream, aeron_term_t * term, gboolean new_term)
{
    aeron_stream_rcv_t * rcv = NULL;
    /
    aeron_pos_t dp = { 0, 0 };
    /
    aeron_pos_t pdp = stream->high;
    gboolean pdpv = ((stream->flags & AERON_STREAM_FLAGS_HIGH_VALID) != 0);
    /
    aeron_pos_t rp = { 0, 0 };
    /
    aeron_pos_t prp = { 0, 0 };
    gboolean prpv = FALSE;
    guint32 cur_receiver_window = 0;
    /
    guint32 frame_flags = 0;

    if (info->type == HDR_TYPE_SM)
    {
        /
        rcv = aeron_stream_rcv_find(stream, &(pinfo->src), pinfo->srcport);
        if (rcv == NULL)
        {
            rcv = aeron_stream_rcv_add(stream, &(pinfo->src), pinfo->srcport);
        }
        else
        {
            prpv = TRUE;
            prp = rcv->completed;
            cur_receiver_window = rcv->receiver_window;
        }
    }
    switch (info->type)
    {
        case HDR_TYPE_DATA:
        case HDR_TYPE_PAD:
            dp.term_id = info->term_id;
            dp.term_offset = info->term_offset;
            aeron_pos_add_length(&dp, info->length, stream->term_length);
            if (pdpv)
            {
                if (dp.term_id > stream->high.term_id)
                {
                    stream->high.term_id = dp.term_id;
                    stream->high.term_offset = dp.term_offset;
                }
                else if (dp.term_offset > stream->high.term_offset)
                {
                    stream->high.term_offset = dp.term_offset;
                }
            }
            else
            {
                stream->flags |= AERON_STREAM_FLAGS_HIGH_VALID;
                stream->high.term_id = dp.term_id;
                stream->high.term_offset = dp.term_offset;
            }
            break;
        case HDR_TYPE_SM:
            rp.term_id = info->term_id;
            rp.term_offset = info->term_offset;
            if (prpv)
            {
                if (rp.term_id > rcv->completed.term_id)
                {
                    rcv->completed.term_id = rp.term_id;
                    rcv->completed.term_offset = rp.term_offset;
                }
                else if (rp.term_offset > rcv->completed.term_offset)
                {
                    rcv->completed.term_offset = rp.term_offset;
                }
            }
            else
            {
                rcv->completed.term_id = rp.term_id;
                rcv->completed.term_offset = rp.term_offset;
            }
            rcv->receiver_window = info->receiver_window;
            break;
        default:
            break;
    }
    if (aeron_stream_analysis)
    {
        if ((stream->flags & AERON_STREAM_FLAGS_HIGH_VALID) != 0)
        {
            finfo->stream_analysis = wmem_new0(wmem_file_scope(), aeron_stream_analysis_t);
        }
    }
    if (finfo->stream_analysis != NULL)
    {
        switch (info->type)
        {
            case HDR_TYPE_DATA:
            case HDR_TYPE_SM:
            case HDR_TYPE_PAD:
                finfo->stream_analysis->high.term_id = stream->high.term_id;
                finfo->stream_analysis->high.term_offset = stream->high.term_offset;
                if (rcv != NULL)
                {
                    finfo->stream_analysis->flags2 |= AERON_STREAM_ANALYSIS_FLAGS2_RCV_VALID;
                    finfo->stream_analysis->completed.term_id = rcv->completed.term_id;
                    finfo->stream_analysis->completed.term_offset = rcv->completed.term_offset;
                    finfo->stream_analysis->receiver_window = rcv->receiver_window;
                    finfo->stream_analysis->outstanding_bytes = aeron_pos_delta(&(finfo->stream_analysis->high), &(finfo->stream_analysis->completed), stream->term_length);
                    if (finfo->stream_analysis->outstanding_bytes >= finfo->stream_analysis->receiver_window)
                    {
                        finfo->stream_analysis->flags |= AERON_STREAM_ANALYSIS_FLAGS_WINDOW_FULL;
                    }
                }
                else
                {
                    finfo->stream_analysis->completed.term_id = 0;
                    finfo->stream_analysis->completed.term_offset = 0;
                    finfo->stream_analysis->receiver_window = 0;
                    finfo->stream_analysis->outstanding_bytes = 0;
                }
                break;
            default:
                break;
        }
        switch (info->type)
        {
            case HDR_TYPE_DATA:
            case HDR_TYPE_PAD:
                if (pdpv)
                {
                    /
                    int rc = aeron_pos_compare(&dp, &pdp);
                    if (rc == 0)
                    {
                        /
                        if (info->length == 0)
                        {
                            finfo->stream_analysis->flags |= AERON_STREAM_ANALYSIS_FLAGS_KEEPALIVE;
                            frame_flags |= AERON_FRAME_INFO_FLAGS_KEEPALIVE;
                        }
                        else
                        {
                            if (prpv)
                            {
                                /
                                if (aeron_pos_compare(&dp, &prp) == 0)
                                {
                                    finfo->stream_analysis->flags |= AERON_STREAM_ANALYSIS_FLAGS_IDLE_RX;
                                }
                                else
                                {
                                    finfo->stream_analysis->flags |= AERON_STREAM_ANALYSIS_FLAGS_PACING_RX;
                                }
                            }
                            else
                            {
                                finfo->stream_analysis->flags |= AERON_STREAM_ANALYSIS_FLAGS_IDLE_RX;
                            }
                            frame_flags |= AERON_FRAME_INFO_FLAGS_RETRANSMISSION;
                        }
                    }
                    else
                    {
                        aeron_pos_t expected_dp;
                        int erc;

                        expected_dp.term_id = pdp.term_id;
                        expected_dp.term_offset = pdp.term_offset;
                        aeron_pos_add_length(&expected_dp, info->length, stream->term_length);
                        erc = aeron_pos_compare(&expected_dp, &dp);
                        if (erc > 0)
                        {
                            /
                            finfo->stream_analysis->flags |= AERON_STREAM_ANALYSIS_FLAGS_RX;
                            frame_flags |= AERON_FRAME_INFO_FLAGS_RETRANSMISSION;
                            aeron_frame_process_rx(info, finfo, term);
                        }
                        else if (erc < 0)
                        {
                            finfo->stream_analysis->flags |= AERON_STREAM_ANALYSIS_FLAGS_OOO_GAP;
                        }
                    }
                }
                if (new_term && (info->term_offset == 0))
                {
                    finfo->stream_analysis->flags |= AERON_STREAM_ANALYSIS_FLAGS_TERM_ID_CHANGE;
                }
                break;
            case HDR_TYPE_SM:
                if (prpv)
                {
                    int rc = aeron_pos_compare(&rp, &prp);
                    if (rc == 0)
                    {
                        /
                       finfo->stream_analysis->flags |= AERON_STREAM_ANALYSIS_FLAGS_KEEPALIVE_SM;
                    }
                    else if (rc < 0)
                    {
                        finfo->stream_analysis->flags |= AERON_STREAM_ANALYSIS_FLAGS_OOO_SM;
                    }
                    if (cur_receiver_window != finfo->stream_analysis->receiver_window)
                    {
                        finfo->stream_analysis->flags |= AERON_STREAM_ANALYSIS_FLAGS_WINDOW_RESIZE;
                    }
                }
                break;
            default:
                break;
        }
    }
    if ((info->type == HDR_TYPE_DATA) || (info->type == HDR_TYPE_PAD))
    {
        aeron_fragment_t * fragment;

        fragment = aeron_term_fragment_find(term, info->term_offset);
        if (fragment == NULL)
        {
            fragment = aeron_term_fragment_add(term, info->term_offset, info->length, info->data_length);
        }
        aeron_fragment_frame_add(fragment, finfo, frame_flags, info->length);
    }
    else
    {
        aeron_term_frame_add(term, finfo, frame_flags);
    }
}
