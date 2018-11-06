static void
dissect_sdp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    proto_tree *sdp_tree;
    proto_item *ti, *sub_ti;
    gint        offset = 0, next_offset, n, i;
    int         linelen;
    gboolean    in_media_description;
    guchar      type, delim;
    int         datalen, tokenoffset, hf = -1;
    char       *string;

    transport_info_t  local_transport_info;
    transport_info_t* transport_info = NULL;
    disposable_media_info_t media_info;

    sdp_packet_info  *sdp_pi;
    struct srtp_info *srtp_info = NULL;

    DPRINT2(("----------------------- dissect_sdp ------------------------"));

    /
    sdp_pi = wmem_new(wmem_packet_scope(), sdp_packet_info);
    sdp_pi->summary_str[0] = '\0';

    if (!pinfo->fd->flags.visited) {
        transport_info = (transport_info_t*)wmem_tree_lookup32( sdp_transport_reqs, pinfo->fd->num );

        if (transport_info == NULL) {
          /
          transport_info = (transport_info_t*)wmem_tree_lookup32( sdp_transport_rsps, pinfo->fd->num );
        }
    }

    if (transport_info == NULL) {
      transport_info = &local_transport_info;
    }
#ifdef DEBUG_CONVERSATION
    else {
        DPRINT(("found previous transport_info:"));
        sdp_dump_transport_info(transport_info);
    }
#endif

    /
    memset(&local_transport_info, 0, sizeof(local_transport_info));
    local_transport_info.media_count = -1;

    for (n = 0; n < SDP_NO_OF_PT; n++) {
        local_transport_info.encoding_name[n] = (char*)UNKNOWN_ENCODING;
    }
    for (n = 0; n < SDP_MAX_RTP_CHANNELS; n++) {
        local_transport_info.media[n].rtp_dyn_payload = rtp_dyn_payload_new();
        local_transport_info.media[n].set_rtp = FALSE;
    }

    memset(&media_info, 0, sizeof(media_info));

    /
    col_append_str(pinfo->cinfo, COL_PROTOCOL, "/SDP");

    /
    /

    ti = proto_tree_add_item(tree, proto_sdp, tvb, offset, -1, ENC_NA);
    sdp_tree = proto_item_add_subtree(ti, ett_sdp);

    /
    in_media_description = FALSE;

    while (tvb_offset_exists(tvb, offset)) {
        /
        linelen = tvb_find_line_end_unquoted(tvb, offset, -1, &next_offset);

        /
        if (linelen < 2)
            break;

        type  = tvb_get_guint8(tvb, offset);
        delim = tvb_get_guint8(tvb, offset + 1);
        if (delim != '=') {
            proto_item *ti2 = proto_tree_add_item(sdp_tree, hf_invalid, tvb, offset, linelen, ENC_UTF_8|ENC_NA);
            expert_add_info(pinfo, ti2, &ei_sdp_invalid_line_equal);
            offset = next_offset;
            continue;
        }

        /
        switch (type) {
            case 'v':
                hf = hf_protocol_version;
                break;
            case 'o':
                hf = hf_owner;
                break;
            case 's':
                hf = hf_session_name;
                break;
            case 'i':
                if (in_media_description) {
                  hf = hf_media_title;
                } else {
                  hf = hf_session_info;
                }
                break;
            case 'u':
                hf = hf_uri;
                break;
            case 'e':
                hf = hf_email;
                break;
            case 'p':
                hf = hf_phone;
                break;
            case 'c':
                hf = hf_connection_info;
                break;
            case 'b':
                hf = hf_bandwidth;
                break;
            case 't':
                hf = hf_time;
                break;
            case 'r':
                hf = hf_repeat_time;
                break;
            case 'm':
                hf = hf_media;

                /
                if (local_transport_info.media_count < (SDP_MAX_RTP_CHANNELS-1))
                    local_transport_info.media_count++;

                if (in_media_description && (media_info.media_count < (SDP_MAX_RTP_CHANNELS-1)))
                    media_info.media_count++;

                in_media_description = TRUE;
                break;
            case 'k':
                hf = hf_encryption_key;
                break;
            case 'a':
                if (in_media_description) {
                    hf = hf_media_attribute;
                } else {
                    hf = hf_session_attribute;
                }
                break;
            case 'z':
                hf = hf_timezone;
                break;
            default:
                hf = hf_unknown;
                break;
        }
        tokenoffset = 2;
        if (hf == hf_unknown)
            tokenoffset = 0;
        string = (char*)tvb_get_string_enc(wmem_packet_scope(), tvb, offset + tokenoffset,
                                                 linelen - tokenoffset, ENC_ASCII);
        sub_ti = proto_tree_add_string(sdp_tree, hf, tvb, offset, linelen,
                                       string);

        call_sdp_subdissector(tvb_new_subset_length(tvb, offset + tokenoffset,
                                             linelen - tokenoffset),
                              pinfo,
                              hf, sub_ti, linelen-tokenoffset,
                              &local_transport_info, &media_info);

        offset = next_offset;
    }

    if (in_media_description) {
        /
        if (local_transport_info.media_count < (SDP_MAX_RTP_CHANNELS-1))
            local_transport_info.media_count++;
        if (media_info.media_count < (SDP_MAX_RTP_CHANNELS-1))
            media_info.media_count++;
    }

    /
    if (transport_info == &local_transport_info) {
        DPRINT(("no previous transport_info saved, calling convert_disposable_media()"));
        DINDENT();
        convert_disposable_media(transport_info, &media_info, 0);
        DENDENT();
#ifdef DEBUG_CONVERSATION
        sdp_dump_transport_info(transport_info);
#endif
    }
#ifdef DEBUG_CONVERSATION
    else {
        DPRINT(("not overwriting previous transport_info, local_transport_info contents:"));
        sdp_dump_transport_info(&local_transport_info);
    }
#endif

    for (n = 0; n < local_transport_info.media_count; n++)
    {
        /
        /
        guint32 current_rtp_port = 0;

        if ((!pinfo->fd->flags.visited) && (transport_info == &local_transport_info) &&
            (transport_info->media_port[n] != 0) &&
            (transport_info->proto_bitmask[n] & (SDP_RTP_PROTO|SDP_SRTP_PROTO)) &&
            (transport_info->proto_bitmask[n] & (SDP_IPv4|SDP_IPv6))) {

            if (transport_info->proto_bitmask[n] & SDP_SRTP_PROTO) {
                srtp_info = wmem_new0(wmem_file_scope(), struct srtp_info);
                if (transport_info->encryption_algorithm != SRTP_ENC_ALG_NOT_SET) {
                    srtp_info->encryption_algorithm = transport_info->encryption_algorithm;
                    srtp_info->auth_algorithm       = transport_info->auth_algorithm;
                    srtp_info->mki_len              = transport_info->mki_len;
                    srtp_info->auth_tag_len         = transport_info->auth_tag_len;
                }
                DPRINT(("calling srtp_add_address for media_port=%d, for channel=%d",
                        transport_info->media_port[n],n));
                DINDENT();
                srtp_add_address(pinfo, &transport_info->src_addr[n], transport_info->media_port[n], 0, "SDP", pinfo->fd->num,
                                (transport_info->proto_bitmask[n] & SDP_VIDEO) ? TRUE : FALSE,
                                 transport_info->media[n].rtp_dyn_payload, srtp_info);
                DENDENT();
            } else {
                DPRINT(("calling rtp_add_address for media_port=%d, for channel=%d",
                        transport_info->media_port[n],n));
                DINDENT();
                rtp_add_address(pinfo, &transport_info->src_addr[n], transport_info->media_port[n], 0, "SDP", pinfo->fd->num,
                                (transport_info->proto_bitmask[n] & SDP_VIDEO) ? TRUE : FALSE,
                                transport_info->media[n].rtp_dyn_payload);
                DENDENT();
            }
            transport_info->media[n].set_rtp = TRUE;
            /
            current_rtp_port = transport_info->media_port[n];

            if (rtcp_handle) {
                if (transport_info->proto_bitmask[n] & SDP_SRTP_PROTO) {
                    DPRINT(("calling srtcp_add_address for media_port=%d, for channel=%d",
                            transport_info->media_port[n],n));
                    DINDENT();
                    srtcp_add_address(pinfo, &transport_info->src_addr[n], transport_info->media_port[n]+1, 0, "SDP", pinfo->fd->num, srtp_info);
                    DENDENT();
                } else {
                    DPRINT(("calling rtcp_add_address for media_port=%d, for channel=%d",
                            transport_info->media_port[n],n));
                    DINDENT();
                    rtcp_add_address(pinfo, &transport_info->src_addr[n], transport_info->media_port[n]+1, 0, "SDP", pinfo->fd->num);
                    DENDENT();
                }
            }
        }

        /
        /
        if ((!pinfo->fd->flags.visited) && (transport_info == &local_transport_info) &&
            (transport_info->proto_bitmask[n] & SDP_SPRT_PROTO) &&
            (transport_info->proto_bitmask[n] & (SDP_IPv4|SDP_IPv6)) &&
            (sprt_handle)) {

            if (transport_info->media_port[n] == 0 && current_rtp_port) {
                sprt_add_address(pinfo, &transport_info->src_addr[n], current_rtp_port,
                                 0, "SDP", pinfo->fd->num); /
            } else {
                sprt_add_address(pinfo, &transport_info->src_addr[n], transport_info->media_port[n], 0, "SDP", pinfo->fd->num);
            }
        }

        /
        /
        if ((!pinfo->fd->flags.visited) && (transport_info == &local_transport_info) &&
            (transport_info->media_port[n] != 0) &&
            !transport_info->media[n].set_rtp &&
            (transport_info->proto_bitmask[n] & SDP_T38_PROTO) &&
            (transport_info->proto_bitmask[n] & SDP_IPv4)) {
            t38_add_address(pinfo, &transport_info->src_addr[n], transport_info->media_port[n], 0, "SDP", pinfo->fd->num);
        }

        /
        /
        if ((!pinfo->fd->flags.visited) && (transport_info == &local_transport_info) &&
            (transport_info->proto_bitmask[n] & SDP_MSRP_PROTO) &&
            (transport_info->proto_bitmask[n] & SDP_MSRP_IPv4) &&
            msrp_handle) {
            msrp_add_address(pinfo, &transport_info->src_addr[n], transport_info->media_port[n], "SDP", pinfo->fd->num);
        }

        if (local_transport_info.media_port[n] != 0) {
            /
            for (i = 0; i < local_transport_info.media[n].pt_count; i++)
            {
                DPRINT(("in for-loop for voip call analysis setting for media #%d, pt=%d",
                        i, local_transport_info.media[n].pt[i]));
                /
                if ((local_transport_info.media[n].pt[i] >= 96) && (local_transport_info.media[n].pt[i] <= 127)) {
                    const gchar *payload_type_str = rtp_dyn_payload_get_name(
                        local_transport_info.media[n].rtp_dyn_payload,
                        local_transport_info.media[n].pt[i]);
                    if (payload_type_str) {
                        if (strlen(sdp_pi->summary_str))
                            g_strlcat(sdp_pi->summary_str, " ", 50);
                        g_strlcat(sdp_pi->summary_str, payload_type_str, 50);
                    } else {
                        char num_pt[10];
                        g_snprintf(num_pt, 10, "%u", local_transport_info.media[n].pt[i]);
                        if (strlen(sdp_pi->summary_str))
                            g_strlcat(sdp_pi->summary_str, " ", 50);
                        g_strlcat(sdp_pi->summary_str, num_pt, 50);
                      }
                } else {
                    if (strlen(sdp_pi->summary_str))
                        g_strlcat(sdp_pi->summary_str, " ", 50);
                    g_strlcat(sdp_pi->summary_str,
                              val_to_str_ext(local_transport_info.media[n].pt[i], &rtp_payload_type_short_vals_ext, "%u"),
                              50);
                }
            }
        }

        /
        /
        if ((transport_info == &local_transport_info) &&
            !transport_info->media[n].set_rtp)
        {
            rtp_dyn_payload_free(transport_info->media[n].rtp_dyn_payload);
            transport_info->media[n].rtp_dyn_payload = NULL;
        }

        /
        if ((local_transport_info.media_port[n] != 0) &&
            (local_transport_info.proto_bitmask[n] & SDP_T38_PROTO)) {
            if (strlen(sdp_pi->summary_str))
                g_strlcat(sdp_pi->summary_str, " ", 50);
            g_strlcat(sdp_pi->summary_str, "t38", 50);
        }
    }

    /
    /
    if (transport_info == &local_transport_info) {
        for (n = transport_info->media_count; n < SDP_MAX_RTP_CHANNELS; n++)
        {
            if (!transport_info->media[n].set_rtp)
            {
                rtp_dyn_payload_free(transport_info->media[n].rtp_dyn_payload);
                transport_info->media[n].rtp_dyn_payload = NULL;
            }
        }
    }

    datalen = tvb_captured_length_remaining(tvb, offset);
    if (datalen > 0) {
        proto_tree_add_item(sdp_tree, hf_sdp_data, tvb, offset, datalen, ENC_NA);
    }
    /
    tap_queue_packet(sdp_tap, pinfo, sdp_pi);
}