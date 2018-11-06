static void
dissect_mmse(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint8 pdut,
        const char *message_type)
{
    guint        offset;
    guint8       field = 0;
    const char   *strval;
    guint        length;
    guint        count;
    guint8       version = 0x80; /

    /
    proto_item  *ti = NULL;
    proto_tree  *mmse_tree = NULL;

    DebugLog(("dissect_mmse() - START (Packet %u)\n", pinfo->num));

    /
    if (tree) {
        DebugLog(("tree != NULL\n"));

        ti = proto_tree_add_item(tree, proto_mmse, tvb, 0, -1, ENC_NA);
        proto_item_append_text(ti, ", Type: %s", message_type);
        /
        mmse_tree = proto_item_add_subtree(ti, ett_mmse);

        /
        proto_tree_add_uint(mmse_tree, hf_mmse_message_type, tvb, 0, 2, pdut);
    }

    offset = 2;                 /

    /
    if (tree || pdu_has_content(pdut)) {
        while ((offset < tvb_reported_length(tvb)) &&
               (field = tvb_get_guint8(tvb, offset++)) != MM_CTYPE_HDR)
        {
            DebugLog(("\tField =  0x%02X (offset = %u): %s\n",
                        field, offset,
                        val_to_str(field, vals_mm_header_names,
                            "Unknown MMS header 0x%02X")));
            switch (field)
            {
                case MM_TID_HDR:                /
                    length = get_text_string(tvb, offset, &strval);
                    if (tree) {
                        proto_tree_add_string(mmse_tree, hf_mmse_transaction_id,
                                tvb, offset - 1, length + 1,strval);
                    }
                    offset += length;
                    break;
                case MM_VERSION_HDR:            /
                    version = tvb_get_guint8(tvb, offset++);
                    if (tree) {
                        guint8   major, minor;
                        char    *vers_string;

                        major = (version & 0x70) >> 4;
                        minor = version & 0x0F;
                        if (minor == 0x0F)
                            vers_string = wmem_strdup_printf(wmem_packet_scope(), "%u", major);
                        else
                            vers_string = wmem_strdup_printf(wmem_packet_scope(), "%u.%u", major, minor);
                        proto_tree_add_string(mmse_tree, hf_mmse_mms_version,
                                tvb, offset - 2, 2, vers_string);
                    }
                    break;
                case MM_BCC_HDR:                /
                    length = get_encoded_strval(tvb, offset, &strval);
                    if (tree) {
                        proto_tree_add_string(mmse_tree, hf_mmse_bcc, tvb,
                                offset - 1, length + 1, strval);
                    }
                    offset += length;
                    break;
                case MM_CC_HDR:                 /
                    length = get_encoded_strval(tvb, offset, &strval);
                    if (tree) {
                        proto_tree_add_string(mmse_tree, hf_mmse_cc, tvb,
                                offset - 1, length + 1, strval);
                    }
                    offset += length;
                    break;
                case MM_CLOCATION_HDR:          /
                    if (pdut == PDU_M_MBOX_DELETE_CONF) {
                        /
                        length = tvb_get_guint8(tvb, offset);
                        if (length == 0x1F) {
                            guint length_len = 0;
                            length = tvb_get_guintvar(tvb, offset + 1,
                                    &length_len);
                            length += 1 + length_len;
                        } else {
                            length += 1;
                        }
                        proto_tree_add_string(mmse_tree,
                                    hf_mmse_content_location,
                                    tvb, offset - 1, length + 1,
                                    "<Undecoded value for m-mbox-delete-conf>");
                    } else {
                        length = get_text_string(tvb, offset, &strval);
                        if (tree) {
                            proto_tree_add_string(mmse_tree,
                                    hf_mmse_content_location,
                                    tvb, offset - 1, length + 1, strval);
                        }
                    }
                    offset += length;
                    break;
                case MM_DATE_HDR:               /
                    {
                        guint            tval;
                        nstime_t         tmptime;

                        tval = get_long_integer(tvb, offset, &count);
                        tmptime.secs = tval;
                        tmptime.nsecs = 0;
                        proto_tree_add_time(mmse_tree, hf_mmse_date, tvb,
                                    offset - 1, count + 1, &tmptime);
                    }
                    offset += count;
                    break;
                case MM_DREPORT_HDR:            /
                    field = tvb_get_guint8(tvb, offset++);
                    if (tree) {
                        proto_tree_add_uint(mmse_tree,
                                hf_mmse_delivery_report,
                                tvb, offset - 2, 2, field);
                    }
                    break;
                case MM_DTIME_HDR:
                    /
                    length = get_value_length(tvb, offset, &count);
                    field = tvb_get_guint8(tvb, offset + count);
                    if (tree) {
                        guint            tval;
                        nstime_t         tmptime;
                        guint            cnt;

                        tval =  get_long_integer(tvb, offset + count + 1, &cnt);
                        tmptime.secs = tval;
                        tmptime.nsecs = 0;

                        if (field == 0x80)
                            proto_tree_add_time(mmse_tree,
                                    hf_mmse_delivery_time_abs,
                                    tvb, offset - 1,
                                    length + count + 1, &tmptime);
                        else
                            proto_tree_add_time(mmse_tree,
                                    hf_mmse_delivery_time_rel,
                                    tvb, offset - 1,
                                    length + count + 1, &tmptime);
                    }
                    offset += length + count;
                    break;
                case MM_EXPIRY_HDR:
                    /
                    length = get_value_length(tvb, offset, &count);
                    field = tvb_get_guint8(tvb, offset + count);
                    if (tree) {
                        guint            tval;
                        nstime_t         tmptime;
                        guint            cnt;

                        tval = get_long_integer(tvb, offset + count + 1, &cnt);
                        tmptime.secs = tval;
                        tmptime.nsecs = 0;

                        if (field == 0x80)
                            proto_tree_add_time(mmse_tree, hf_mmse_expiry_abs,
                                    tvb, offset - 1,
                                    length + count + 1, &tmptime);
                        else
                            proto_tree_add_time(mmse_tree, hf_mmse_expiry_rel,
                                    tvb, offset - 1,
                                    length + count + 1, &tmptime);
                    }
                    offset += length + count;
                    break;
                case MM_FROM_HDR:
                    /
                    length = get_value_length(tvb, offset, &count);
                    if (tree) {
                        field = tvb_get_guint8(tvb, offset + count);
                        if (field == 0x81) {
                            proto_tree_add_string(mmse_tree, hf_mmse_from, tvb,
                                    offset-1, length + count + 1,
                                    "<insert address>");
                        } else {
                            (void) get_encoded_strval(tvb, offset + count + 1,
                                                      &strval);
                            proto_tree_add_string(mmse_tree, hf_mmse_from, tvb,
                                    offset-1, length + count + 1, strval);
                        }
                    }
                    offset += length + count;
                    break;
                case MM_MCLASS_HDR:
                    /
                    field = tvb_get_guint8(tvb, offset);
                    if (field & 0x80) {
                        offset++;
                        if (tree) {
                            proto_tree_add_uint(mmse_tree,
                                    hf_mmse_message_class_id,
                                    tvb, offset - 2, 2, field);
                        }
                    } else {
                        length = get_text_string(tvb, offset, &strval);
                        if (tree) {
                            proto_tree_add_string(mmse_tree,
                                    hf_mmse_message_class_str,
                                    tvb, offset - 1, length + 1,
                                    strval);
                        }
                        offset += length;
                    }
                    break;
                case MM_MID_HDR:                /
                    length = get_text_string(tvb, offset, &strval);
                    if (tree) {
                        proto_tree_add_string(mmse_tree, hf_mmse_message_id,
                                tvb, offset - 1, length + 1, strval);
                    }
                    offset += length;
                    break;
                case MM_MSIZE_HDR:              /
                    length = get_long_integer(tvb, offset, &count);
                    if (tree) {
                        proto_tree_add_uint(mmse_tree, hf_mmse_message_size,
                                tvb, offset - 1, count + 1, length);
                    }
                    offset += count;
                    break;
                case MM_PRIORITY_HDR:           /
                    field = tvb_get_guint8(tvb, offset++);
                    if (tree) {
                        proto_tree_add_uint(mmse_tree, hf_mmse_priority, tvb,
                                offset - 2, 2, field);
                    }
                    break;
                case MM_RREPLY_HDR:             /
                    field = tvb_get_guint8(tvb, offset++);
                    if (tree) {
                        if (version == 0x80) { /
                            proto_tree_add_uint(mmse_tree, hf_mmse_read_reply,
                                    tvb, offset - 2, 2, field);
                        } else {
                            proto_tree_add_uint(mmse_tree, hf_mmse_read_report,
                                    tvb, offset - 2, 2, field);
                        }
                    }
                    break;
                case MM_RALLOWED_HDR:           /
                    field = tvb_get_guint8(tvb, offset++);
                    if (tree) {
                        proto_tree_add_uint(mmse_tree, hf_mmse_report_allowed,
                                tvb, offset - 2, 2, field);
                    }
                    break;
                case MM_RSTATUS_HDR:
                    field = tvb_get_guint8(tvb, offset++);
                    if (tree) {
                        proto_tree_add_uint(mmse_tree, hf_mmse_response_status,
                                tvb, offset - 2, 2, field);
                    }
                    break;
                case MM_RTEXT_HDR:              /
                    if (pdut == PDU_M_MBOX_DELETE_CONF) {
                        /
                        length = tvb_get_guint8(tvb, offset);
                        if (length == 0x1F) {
                            guint length_len = 0;
                            length = tvb_get_guintvar(tvb, offset + 1,
                                    &length_len);
                            length += 1 + length_len;
                        } else {
                            length += 1;
                        }
                        if (tree) {
                            proto_tree_add_string(mmse_tree,
                                    hf_mmse_content_location,
                                    tvb, offset - 1, length + 1,
                                    "<Undecoded value for m-mbox-delete-conf>");
                        }
                    } else {
                        length = get_encoded_strval(tvb, offset, &strval);
                        if (tree) {
                            proto_tree_add_string(mmse_tree,
                                    hf_mmse_response_text, tvb, offset - 1,
                                    length + 1, strval);
                        }
                    }
                    offset += length;
                    break;
                case MM_SVISIBILITY_HDR:        /
                    field = tvb_get_guint8(tvb, offset++);
                    if (tree) {
                        proto_tree_add_uint(mmse_tree,hf_mmse_sender_visibility,
                                tvb, offset - 2, 2, field);
                    }
                    break;
                case MM_STATUS_HDR:
                    field = tvb_get_guint8(tvb, offset++);
                    if (tree) {
                        proto_tree_add_uint(mmse_tree, hf_mmse_status, tvb,
                                offset - 2, 2, field);
                    }
                    break;
                case MM_SUBJECT_HDR:            /
                    length = get_encoded_strval(tvb, offset, &strval);
                    if (tree) {
                        proto_tree_add_string(mmse_tree, hf_mmse_subject, tvb,
                                offset - 1, length + 1, strval);
                    }
                    offset += length;
                    break;
                case MM_TO_HDR:                 /
                    length = get_encoded_strval(tvb, offset, &strval);
                    if (tree) {
                        proto_tree_add_string(mmse_tree, hf_mmse_to, tvb,
                                offset - 1, length + 1, strval);
                    }
                    offset += length;
                    break;

                /
                case MM_RETRIEVE_STATUS_HDR:    /
                    field = tvb_get_guint8(tvb, offset++);
                    if (tree) {
                        proto_tree_add_uint(mmse_tree, hf_mmse_retrieve_status,
                                tvb, offset - 2, 2, field);
                    }
                    break;
                case MM_RETRIEVE_TEXT_HDR:
                    if (pdut == PDU_M_MBOX_DELETE_CONF) {
                        /
                        length = tvb_get_guint8(tvb, offset);
                        if (length == 0x1F) {
                            guint length_len = 0;
                            length = tvb_get_guintvar(tvb, offset + 1,
                                    &length_len);
                            length += 1 + length_len;
                        } else {
                            length += 1;
                        }
                        if (tree) {
                            proto_tree_add_string(mmse_tree,
                                    hf_mmse_content_location,
                                    tvb, offset - 1, length + 1,
                                    "<Undecoded value for m-mbox-delete-conf>");
                        }
                    } else {
                        /
                        length = get_encoded_strval(tvb, offset, &strval);
                        if (tree) {
                            proto_tree_add_string(mmse_tree,
                                    hf_mmse_retrieve_text, tvb, offset - 1,
                                    length + 1, strval);
                        }
                    }
                    offset += length;
                    break;
                case MM_READ_STATUS_HDR:        /
                    field = tvb_get_guint8(tvb, offset++);
                    if (tree) {
                        proto_tree_add_uint(mmse_tree, hf_mmse_read_status,
                                tvb, offset - 2, 2, field);
                    }
                    break;
                case MM_REPLY_CHARGING_HDR:     /
                    field = tvb_get_guint8(tvb, offset++);
                    if (tree) {
                        proto_tree_add_uint(mmse_tree, hf_mmse_reply_charging,
                                tvb, offset - 2, 2, field);
                    }
                    break;
                case MM_REPLY_CHARGING_DEADLINE_HDR:    /
                    /
                    length = get_value_length(tvb, offset, &count);
                    field = tvb_get_guint8(tvb, offset + count);
                    if (tree) {
                        guint            tval;
                        nstime_t         tmptime;
                        guint            cnt;

                        tval = get_long_integer(tvb, offset + count + 1, &cnt);
                        tmptime.secs = tval;
                        tmptime.nsecs = 0;

                        if (field == 0x80)
                            proto_tree_add_time(mmse_tree, hf_mmse_reply_charging_deadline_abs,
                                    tvb, offset - 1,
                                    length + count + 1, &tmptime);
                        else
                            proto_tree_add_time(mmse_tree, hf_mmse_reply_charging_deadline_rel,
                                    tvb, offset - 1,
                                    length + count + 1, &tmptime);
                    }
                    offset += length + count;
                    break;
                case MM_REPLY_CHARGING_ID_HDR:  /
                    length = get_text_string(tvb, offset, &strval);
                    if (tree) {
                        proto_tree_add_string(mmse_tree,
                                hf_mmse_reply_charging_id,
                                tvb, offset - 1, length + 1, strval);
                    }
                    offset += length;
                    break;
                case MM_REPLY_CHARGING_SIZE_HDR:        /
                    length = get_long_integer(tvb, offset, &count);
                    if (tree) {
                        proto_tree_add_uint(mmse_tree,
                                hf_mmse_reply_charging_size,
                                tvb, offset - 1, count + 1, length);
                    }
                    offset += count;
                    break;
                case MM_PREV_SENT_BY_HDR:
                    /
                    length = get_value_length(tvb, offset, &count);
                    if (tree) {
                        guint32 fwd_count, count1, count2;
                        proto_tree *subtree = NULL;
                        proto_item *tii = NULL;
                        /
                        fwd_count = get_integer_value(tvb, offset + count,
                            &count1);
                        /
                        count2 = get_encoded_strval(tvb,
                                offset + count + count1, &strval);
                        /
                        tii = proto_tree_add_string_format(mmse_tree,
                                hf_mmse_prev_sent_by,
                                tvb, offset - 1, 1 + count + length,
                                strval, "%s (Forwarded-count=%u)",
                                format_text(strval, strlen(strval)),
                                fwd_count);
                        subtree = proto_item_add_subtree(tii,
                                ett_mmse_hdr_details);
                        proto_tree_add_uint(subtree,
                                hf_mmse_prev_sent_by_fwd_count,
                                tvb, offset + count, count1, fwd_count);
                        proto_tree_add_string(subtree,
                                hf_mmse_prev_sent_by_address,
                                tvb, offset + count + count1, count2, strval);
                    }
                    offset += length + count;
                    break;
                case MM_PREV_SENT_DATE_HDR:
                    /
                    length = get_value_length(tvb, offset, &count);
                    if (tree) {
                        guint32 fwd_count, count1, count2;
                        guint            tval;
                        nstime_t         tmptime;
                        proto_tree *subtree = NULL;
                        proto_item *tii = NULL;
                        /
                        fwd_count = get_integer_value(tvb, offset + count,
                            &count1);
                        /
                        tval = get_long_integer(tvb, offset + count + count1,
                                &count2);
                        tmptime.secs = tval;
                        tmptime.nsecs = 0;
                        strval = abs_time_to_str(wmem_packet_scope(), &tmptime, ABSOLUTE_TIME_LOCAL,
                            TRUE);
                        /
                        tii = proto_tree_add_string_format(mmse_tree,
                                hf_mmse_prev_sent_date,
                                tvb, offset - 1, 1 + count + length,
                                strval, "%s (Forwarded-count=%u)",
                                format_text(strval, strlen(strval)),
                                fwd_count);
                        subtree = proto_item_add_subtree(tii,
                                ett_mmse_hdr_details);
                        proto_tree_add_uint(subtree,
                                hf_mmse_prev_sent_date_fwd_count,
                                tvb, offset + count, count1, fwd_count);
                        proto_tree_add_string(subtree,
                                hf_mmse_prev_sent_date_date,
                                tvb, offset + count + count1, count2, strval);
                    }
                    offset += length + count;
                    break;

                /

                default:
                    if (field & 0x80) { /
                        guint8 peek = tvb_get_guint8(tvb, offset);
                        const char *hdr_name = val_to_str(field, vals_mm_header_names,
                                "Unknown field (0x%02x)");
            const char *str;
                        DebugLog(("\t\tUndecoded well-known header: %s\n",
                                    hdr_name));

                        if (peek & 0x80) { /
                            length = 1;
                            if (tree) {
                                proto_tree_add_uint_format(mmse_tree, hf_mmse_header_uint, tvb, offset - 1,
                                        length + 1, peek,
                                        "%s: <Well-known value 0x%02x>"
                                        " (not decoded)",
                                        hdr_name, peek);
                            }
                        } else if ((peek == 0) || (peek >= 0x20)) { /
                            length = get_text_string(tvb, offset, &strval);
                            if (tree) {
                                str = format_text(strval, strlen(strval));
                                proto_tree_add_string_format(mmse_tree, hf_mmse_header_string, tvb, offset - 1,
                                        length + 1, str, "%s: %s (Not decoded)", hdr_name, str);
                            }
                        } else { /
                            if (peek == 0x1F) { /
                                guint length_len = 0;
                                length = 1 + tvb_get_guintvar(tvb, offset + 1,
                                        &length_len);
                                length += length_len;
                            } else { /
                                length = 1 + tvb_get_guint8(tvb, offset);
                            }
                            if (tree) {
                                proto_tree_add_bytes_format(mmse_tree, hf_mmse_header_bytes, tvb, offset - 1,
                                        length + 1, NULL, "%s: "
                                        "<Value in general form> (not decoded)",
                                        hdr_name);
                            }
                        }
                        offset += length;
                    } else { /
                        guint            length2;
                        const char       *strval2;

                        --offset;
                        length = get_text_string(tvb, offset, &strval);
                        DebugLog(("\t\tUndecoded literal header: %s\n",
                                    strval));
                        length2= get_text_string(tvb, offset+length, &strval2);

                        if (tree) {
                            proto_tree_add_string_format(mmse_tree,
                                    hf_mmse_ffheader, tvb, offset,
                                    length + length2,
                                    tvb_get_string_enc(wmem_packet_scope(), tvb, offset,
                                            length + length2, ENC_ASCII),
                                    "%s: %s",
                                    format_text(strval, strlen(strval)),
                                    format_text(strval2, strlen(strval2)));
                        }
                        offset += length + length2;
                    }
                    break;
            }
            DebugLog(("\tEnd(case)\n"));
        }
        DebugLog(("\tEnd(switch)\n"));
        if (field == MM_CTYPE_HDR) {
            /
            tvbuff_t    *tmp_tvb;
            guint        type;
            const char  *type_str;

            DebugLog(("Content-Type: [from WSP dissector]\n"));
            DebugLog(("Calling add_content_type() in WSP dissector\n"));
            offset = add_content_type(mmse_tree, pinfo, tvb, offset, &type, &type_str);
            DebugLog(("Generating new TVB subset (offset = %u)\n", offset));
            tmp_tvb = tvb_new_subset_remaining(tvb, offset);
            DebugLog(("Add POST data\n"));
            add_post_data(mmse_tree, tmp_tvb, type, type_str, pinfo);
            DebugLog(("Done!\n"));
        }
    } else {
        DebugLog(("tree == NULL and PDU has no potential content\n"));
    }

    /
    DebugLog(("dissect_mmse() - END\n"));
}
