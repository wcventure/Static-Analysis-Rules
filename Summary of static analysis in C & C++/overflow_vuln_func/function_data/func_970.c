static int
dissect_hiqnet_pdu(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data _U_)
{
    guint8 headerlen = 0;
    guint32 messagelen = 0;
    guint16 srcdev = 0;
    guint8 srcvdaddr = 0;
    guint8 srcob0addr = 0;
    guint8 srcob1addr = 0;
    guint8 srcob2addr = 0;
    guint16 dstdev = 0;
    guint8 dstvdaddr = 0;
    guint8 dstob0addr = 0;
    guint8 dstob1addr = 0;
    guint8 dstob2addr = 0;
    guint16 messageid = 0;
    guint16 flags = 0;
    guint16 paramcount = 0;
    guint16 subcount = 0;
    guint16 attrcount = 0;
    gint str_len = 0;
    guint16 vdscount = 0;
    guint16 eventscount = 0;
    guint16 objcount = 0;
    guint16 ifacecount = 0;

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "HiQnet");
    /
    col_clear(pinfo->cinfo,COL_INFO);

    srcdev = tvb_get_ntohs(tvb, 6);
    srcvdaddr = tvb_get_guint8(tvb, 8);
    srcob0addr = tvb_get_guint8(tvb, 9);
    srcob1addr = tvb_get_guint8(tvb, 10);
    srcob2addr = tvb_get_guint8(tvb, 11);
    dstdev = tvb_get_ntohs(tvb, 12);
    dstvdaddr = tvb_get_guint8(tvb, 14);
    dstob0addr = tvb_get_guint8(tvb, 15);
    dstob1addr = tvb_get_guint8(tvb, 16);
    dstob2addr = tvb_get_guint8(tvb, 17);
    messageid = tvb_get_ntohs(tvb, 18);
    col_add_fstr(pinfo->cinfo, COL_INFO, "Msg: %s, Src: %u.%u.%u.%u.%u, Dst: %u.%u.%u.%u.%u",
        val_to_str(messageid, messageidnames, "Unknown (0x%04x)"),
        srcdev, srcvdaddr, srcob0addr, srcob1addr, srcob2addr,
        dstdev, dstvdaddr, dstob0addr, dstob1addr, dstob2addr);

    if (tree) { /
        proto_item *ti = NULL;
        proto_item *item = NULL;
        proto_tree *hiqnet_tree = NULL;
        proto_tree *hiqnet_header_tree = NULL;
        proto_tree *hiqnet_session_tree = NULL;
        proto_tree *hiqnet_error_tree = NULL;
        proto_tree *hiqnet_multipart_tree = NULL;
        proto_tree *hiqnet_payload_tree = NULL;
        proto_tree *hiqnet_parameter_tree = NULL;
        proto_tree *hiqnet_attribute_tree = NULL;
        proto_tree *hiqnet_vds_tree = NULL;
        proto_tree *hiqnet_event_tree = NULL;
        proto_tree *hiqnet_subscription_tree = NULL;
        proto_tree *hiqnet_object_tree = NULL;
        proto_tree *hiqnet_ifaces_tree = NULL;
        gint offset = 0;

        messagelen = tvb_get_ntohl(tvb, 2);
        ti = proto_tree_add_item(tree, proto_hiqnet, tvb, 0, messagelen, ENC_NA);
        proto_item_append_text(ti, ", Msg: %s",
            val_to_str(messageid, messageidnames, "Unknown (0x%04x)"));
        proto_item_append_text(ti, ", Src %u.%u.%u.%u.%u",
            srcdev, srcvdaddr, srcob0addr, srcob1addr, srcob2addr);
        proto_item_append_text(ti, ", Dst: %u.%u.%u.%u.%u",
            dstdev, dstvdaddr, dstob0addr, dstob1addr, dstob2addr);
        hiqnet_tree = proto_item_add_subtree(ti, ett_hiqnet);

        /
        headerlen =  tvb_get_guint8(tvb, 1);
        hiqnet_header_tree = proto_tree_add_subtree(hiqnet_tree, tvb, 0, headerlen, ett_hiqnet, NULL, "Header");

        /
        proto_tree_add_item(hiqnet_header_tree, hf_hiqnet_version, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset += 1;
        proto_tree_add_item(hiqnet_header_tree, hf_hiqnet_headerlen, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset += 1;
        proto_tree_add_item(hiqnet_header_tree, hf_hiqnet_messagelen, tvb, offset, 4, ENC_BIG_ENDIAN);
        offset += 4;
        proto_tree_add_item(hiqnet_header_tree, hf_hiqnet_sourcedev, tvb, offset, 2, ENC_BIG_ENDIAN);
        item = proto_tree_add_item(hiqnet_header_tree, hf_hiqnet_devaddr, tvb, offset, 2, ENC_BIG_ENDIAN);
        PROTO_ITEM_SET_HIDDEN(item);
        offset += 2;
        hiqnet_display_vdobjectaddr(hiqnet_header_tree, hf_hiqnet_sourceaddr, tvb, offset);
        offset += 4;
        proto_tree_add_item(hiqnet_header_tree, hf_hiqnet_destdev, tvb, offset, 2, ENC_BIG_ENDIAN);
        item = proto_tree_add_item(hiqnet_header_tree, hf_hiqnet_devaddr, tvb, offset, 2, ENC_BIG_ENDIAN);
        PROTO_ITEM_SET_HIDDEN(item);
        offset += 2;
        hiqnet_display_vdobjectaddr(hiqnet_header_tree, hf_hiqnet_destaddr, tvb, offset);
        offset += 4;
        proto_tree_add_item(hiqnet_header_tree, hf_hiqnet_messageid, tvb, offset, 2, ENC_BIG_ENDIAN);
        offset += 2;
        flags = tvb_get_ntohs(tvb, offset);
        proto_tree_add_bitmask(hiqnet_header_tree, tvb, offset, hf_hiqnet_flags,
                               ett_hiqnet_flags, hiqnet_flag_fields, ENC_BIG_ENDIAN);
        offset += 2;
        proto_tree_add_item(hiqnet_header_tree, hf_hiqnet_hopcnt, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset += 1;
        proto_tree_add_item(hiqnet_header_tree, hf_hiqnet_seqnum, tvb, offset, 2, ENC_BIG_ENDIAN);
        offset += 2;

        /
        if (flags & HIQNET_ERROR_FLAG) {
            /
            hiqnet_error_tree = proto_tree_add_subtree(hiqnet_header_tree, tvb, offset, 2, ett_hiqnet, NULL, "Error");
            proto_tree_add_item(hiqnet_error_tree, hf_hiqnet_errcode, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(hiqnet_error_tree, hf_hiqnet_errstr, tvb, offset, headerlen - offset, ENC_UCS_2);
        }
        if (flags & HIQNET_MULTIPART_FLAG) {
            /
            hiqnet_multipart_tree = proto_tree_add_subtree(hiqnet_header_tree, tvb, offset, 2, ett_hiqnet, NULL, "Multi-part");
            proto_tree_add_item(hiqnet_multipart_tree, hf_hiqnet_startseqno, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;
            proto_tree_add_item(hiqnet_multipart_tree, hf_hiqnet_rembytes, tvb, offset, 4, ENC_BIG_ENDIAN);
            offset += 4;
        }
        if (flags & HIQNET_SESSION_NUMBER_FLAG) {
            hiqnet_session_tree = proto_tree_add_subtree(hiqnet_header_tree, tvb, offset, 2, ett_hiqnet, NULL, "Session");
            proto_tree_add_item(hiqnet_session_tree, hf_hiqnet_sessnum, tvb, offset, 2, ENC_BIG_ENDIAN);
        }

        /
        offset = headerlen; /
        hiqnet_payload_tree = proto_tree_add_subtree(
            hiqnet_tree, tvb, offset, messagelen - headerlen, ett_hiqnet, NULL, "Payload");
        switch(messageid) {
            case HIQNET_DISCOINFO_MSG :
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_devaddr, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_cost, tvb, offset, 1, ENC_BIG_ENDIAN);
                offset += 1;
                offset = hiqnet_display_sernum(hiqnet_payload_tree, tvb, offset);
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_maxmsgsize, tvb, offset, 4, ENC_BIG_ENDIAN);
                offset += 4;
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_keepaliveperiod, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                hiqnet_display_netinfo(hiqnet_payload_tree, tvb, offset);
                break;
            case HIQNET_HELLO_MSG :
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_sessnum, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                proto_tree_add_bitmask(hiqnet_payload_tree, tvb, offset, hf_hiqnet_flagmask,
                               ett_hiqnet_flags, hiqnet_flag_fields, ENC_BIG_ENDIAN);
                break;
            case HIQNET_MULTPARMGET_MSG :
                paramcount = tvb_get_ntohs(tvb, offset);
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_paramcount, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                while (paramcount > 0) {
                    hiqnet_parameter_tree = proto_tree_add_subtree(
                        hiqnet_payload_tree, tvb, offset, -1, ett_hiqnet, NULL, "Parameter");
                    proto_tree_add_item(hiqnet_parameter_tree, hf_hiqnet_paramid, tvb, offset, 2, ENC_BIG_ENDIAN);
                    offset += 2;
                    if (flags & HIQNET_INFO_FLAG) { /
                        offset = hiqnet_display_data(hiqnet_parameter_tree, tvb, offset);
                    }
                    paramcount -= 1;
                }
                break;
            case HIQNET_MULTPARMSET_MSG :
                paramcount = tvb_get_ntohs(tvb, offset);
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_paramcount, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                while (paramcount > 0) {
                    hiqnet_parameter_tree = proto_tree_add_subtree(
                        hiqnet_payload_tree, tvb, offset, -1, ett_hiqnet, NULL, "Parameter");
                    proto_tree_add_item(hiqnet_parameter_tree, hf_hiqnet_paramid, tvb, offset, 2, ENC_BIG_ENDIAN);
                    offset += 2;
                    offset = hiqnet_display_data(hiqnet_parameter_tree, tvb, offset);
                    paramcount -= 1;
                }
                break;
            case HIQNET_PARMSUBALL_MSG :
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_devaddr, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                hiqnet_display_vdobjectaddr(hiqnet_payload_tree, hf_hiqnet_vdobject, tvb, offset);
                offset += 4;
                /
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_subtype, tvb, offset, 1, ENC_BIG_ENDIAN);
                offset += 1;
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_sensrate, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                /
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_subflags, tvb, offset, 2, ENC_BIG_ENDIAN);
                break;
            case HIQNET_PARMUNSUBALL_MSG : /
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_devaddr, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                hiqnet_display_vdobjectaddr(hiqnet_payload_tree, hf_hiqnet_vdobject, tvb, offset);
                offset += 4;
                /
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_subtype, tvb, offset, 1, ENC_BIG_ENDIAN);
                break;
            case HIQNET_MULTPARMSUB_MSG :
                /
                subcount = tvb_get_ntohs(tvb, offset);
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_subcount, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                while (subcount > 0) {
                    hiqnet_subscription_tree = proto_tree_add_subtree(
                        hiqnet_payload_tree, tvb, offset, -1, ett_hiqnet, NULL, "Subscription");
                    offset = hiqnet_display_paramsub(hiqnet_subscription_tree, tvb, offset);
                    subcount -= 1;
                }
                break;
            case HIQNET_GOODBYE_MSG :
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_devaddr, tvb, offset, 2, ENC_BIG_ENDIAN);
                break;
            case HIQNET_GETATTR_MSG :
                attrcount = tvb_get_ntohs(tvb, offset);
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_attrcount, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                if (flags & HIQNET_INFO_FLAG) { /
                    while (attrcount > 0) {
                        hiqnet_attribute_tree = proto_tree_add_subtree(
                            hiqnet_payload_tree, tvb, offset, -1, ett_hiqnet, NULL, "Attribute");
                        proto_tree_add_item(hiqnet_attribute_tree, hf_hiqnet_attrid, tvb, offset, 2, ENC_BIG_ENDIAN);
                        offset += 2;
                        offset = hiqnet_display_data(hiqnet_attribute_tree, tvb, offset);
                        attrcount -= 1;
                    }
                } else { /
                    while (attrcount > 0) {
                        proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_attrid, tvb, offset, 2, ENC_BIG_ENDIAN);
                        offset += 2;
                        attrcount -= 1;
                    }
                }
                break;
            case HIQNET_GETVDLIST_MSG :
                /
                str_len = tvb_get_ntohs(tvb, offset);
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_strlen, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_wrkgrppath, tvb, offset, str_len, ENC_UCS_2);
                offset += str_len;
                if (flags & HIQNET_INFO_FLAG) { /
                    vdscount = tvb_get_ntohs(tvb, offset);
                    proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_numvds, tvb, offset, 2, ENC_BIG_ENDIAN);
                    offset += 2;
                    while (vdscount > 0) {
                        hiqnet_vds_tree = proto_tree_add_subtree(
                            hiqnet_payload_tree, tvb, offset, -1, ett_hiqnet, NULL, "Virtual Devices");
                        proto_tree_add_item(hiqnet_vds_tree, hf_hiqnet_vdaddr, tvb, offset, 1, ENC_BIG_ENDIAN);
                        offset += 1;
                        proto_tree_add_item(hiqnet_vds_tree, hf_hiqnet_vdclassid, tvb, offset, 2, ENC_BIG_ENDIAN);
                        offset += 2;
                        vdscount -= 1;
                    }
                }
                break;
            case HIQNET_STORE_MSG :
                /
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_stract, tvb, offset, 1, ENC_BIG_ENDIAN);
                offset += 1;
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_strnum, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                str_len = tvb_get_ntohs(tvb, offset);
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_strlen, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_wrkgrppath, tvb, offset, str_len, ENC_UCS_2);
                offset += str_len;
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_scope, tvb, offset, 1, ENC_BIG_ENDIAN);
                break;
            case HIQNET_RECALL_MSG :
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_recact, tvb, offset, 1, ENC_BIG_ENDIAN);
                offset += 1;
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_recnum, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                str_len = tvb_get_ntohs(tvb, offset);
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_strlen, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_wrkgrppath, tvb, offset, str_len, ENC_UCS_2);
                offset += str_len;
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_scope, tvb, offset, 1, ENC_BIG_ENDIAN);
                break;
            case HIQNET_LOCATE_MSG :
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_time, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                hiqnet_display_sernum(hiqnet_payload_tree, tvb, offset);
                break;
            case HIQNET_SUBEVTLOGMSGS_MSG :
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_maxdatasize, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                proto_tree_add_bitmask(hiqnet_payload_tree, tvb, offset, hf_hiqnet_catfilter,
                               ett_hiqnet_cats, hiqnet_cat_fields, ENC_BIG_ENDIAN);
                break;
            case HIQNET_UNSUBEVTLOGMSGS_MSG :
                proto_tree_add_bitmask(hiqnet_payload_tree, tvb, offset, hf_hiqnet_catfilter,
                               ett_hiqnet_cats, hiqnet_cat_fields, ENC_BIG_ENDIAN);
                break;
            case HIQNET_REQEVTLOG_MSG :
                /
                if (flags & HIQNET_INFO_FLAG) { /
                    eventscount = tvb_get_ntohs(tvb, offset);
                    proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_entrieslen, tvb, offset, 2, ENC_BIG_ENDIAN);
                    offset += 2;
                    while (eventscount > 0) {
                        hiqnet_event_tree = proto_tree_add_subtree(
                            hiqnet_payload_tree, tvb, offset, -1, ett_hiqnet, NULL, "Event");

                        proto_tree_add_item(hiqnet_event_tree, hf_hiqnet_category, tvb, offset, 2, ENC_BIG_ENDIAN);
                        offset += 2;

                        proto_tree_add_item(hiqnet_event_tree, hf_hiqnet_eventid, tvb, offset, 2, ENC_BIG_ENDIAN);
                        offset += 2;
                        proto_tree_add_item(hiqnet_event_tree, hf_hiqnet_priority, tvb, offset, 1, ENC_BIG_ENDIAN);
                        offset += 1;
                        proto_tree_add_item(hiqnet_event_tree, hf_hiqnet_eventseqnum, tvb, offset, 4, ENC_BIG_ENDIAN);
                        offset += 4;
                        str_len = tvb_get_ntohs(tvb, offset);
                        proto_tree_add_item(hiqnet_event_tree, hf_hiqnet_eventtime, tvb, offset, str_len, ENC_UCS_2);
                        offset += str_len;
                        str_len = tvb_get_ntohs(tvb, offset);
                        proto_tree_add_item(hiqnet_event_tree, hf_hiqnet_eventdate, tvb, offset, str_len, ENC_UCS_2);
                        offset += str_len;
                        str_len = tvb_get_ntohs(tvb, offset);
                        proto_tree_add_item(hiqnet_event_tree, hf_hiqnet_eventinfo, tvb, offset, str_len, ENC_UCS_2);
                        offset += str_len;
                        str_len = tvb_get_ntohs(tvb, offset);
                        proto_tree_add_item(
                            hiqnet_event_tree, hf_hiqnet_eventadddata, tvb, offset, str_len, ENC_NA);
                        offset += str_len;
                        eventscount -= 1;
                    }
                }
                break;
            case HIQNET_MULTPARMUNSUB_MSG :
                /
                subcount = tvb_get_ntohs(tvb, offset);
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_subcount, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                while (subcount > 0) {
                    hiqnet_subscription_tree = proto_tree_add_subtree(
                        hiqnet_payload_tree, tvb, offset, -1, ett_hiqnet, NULL, "Subscription");
                    proto_tree_add_item(hiqnet_subscription_tree, hf_hiqnet_pubparmid, tvb, offset, 2, ENC_BIG_ENDIAN);
                    offset += 2;
                    proto_tree_add_item(hiqnet_subscription_tree, hf_hiqnet_subparmid, tvb, offset, 2, ENC_BIG_ENDIAN);
                    offset += 2;
                    subcount -= 1;
                }
                break;
            case HIQNET_MULTOBJPARMSET_MSG :
                /
                objcount = tvb_get_ntohs(tvb, offset);
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_objcount, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                while (objcount > 0) {
                    hiqnet_object_tree = proto_tree_add_subtree(
                        hiqnet_payload_tree, tvb, offset, -1, ett_hiqnet, NULL, "Object");
                    hiqnet_display_vdobjectaddr(hiqnet_header_tree, hf_hiqnet_vdobject, tvb, offset);
                    offset += 4;
                    paramcount = tvb_get_ntohs(tvb, offset);
                    proto_tree_add_item(hiqnet_object_tree, hf_hiqnet_paramcount, tvb, offset, 2, ENC_BIG_ENDIAN);
                    offset += 2;
                    while (paramcount > 0) {
                        hiqnet_parameter_tree = proto_tree_add_subtree(
                            hiqnet_object_tree, tvb, offset, -1, ett_hiqnet, NULL, "Parameter");
                        proto_tree_add_item(hiqnet_parameter_tree, hf_hiqnet_paramid, tvb, offset, 2, ENC_BIG_ENDIAN);
                        offset += 2;
                        offset = hiqnet_display_data(hiqnet_parameter_tree, tvb, offset);
                        paramcount -= 1;
                    }
                    objcount -= 1;
                }
                break;
            case HIQNET_PARMSETPCT_MSG :
                /
                paramcount = tvb_get_ntohs(tvb, offset);
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_paramcount, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                while (paramcount > 0) {
                    hiqnet_parameter_tree = proto_tree_add_subtree(
                        hiqnet_payload_tree, tvb, offset, -1, ett_hiqnet, NULL, "Parameter");
                    proto_tree_add_item(hiqnet_parameter_tree, hf_hiqnet_paramid, tvb, offset, 2, ENC_BIG_ENDIAN);
                    offset += 2;
                    /
                    proto_tree_add_item(hiqnet_parameter_tree, hf_hiqnet_paramval, tvb, offset, 2, ENC_BIG_ENDIAN);
                    offset += 2;
                    paramcount -= 1;
                }
                break;
            case HIQNET_PARMSUBPCT_MSG :
                /
                subcount = tvb_get_ntohs(tvb, offset);
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_subcount, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                while (subcount > 0) {
                    hiqnet_subscription_tree = proto_tree_add_subtree(
                        hiqnet_payload_tree, tvb, offset, -1, ett_hiqnet, NULL, "Subscription");
                    offset = hiqnet_display_paramsub(hiqnet_subscription_tree, tvb, offset);
                    subcount -= 1;
                }
                break;
            case HIQNET_GETNETINFO_MSG :
                /
                offset = hiqnet_display_sernum(hiqnet_payload_tree, tvb, offset);
                if (flags & HIQNET_INFO_FLAG) { /
                    ifacecount = tvb_get_ntohs(tvb, offset);
                    proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_ifacecount, tvb, offset, 2, ENC_BIG_ENDIAN);
                    offset += 2;
                    while (ifacecount > 0) {
                        hiqnet_ifaces_tree = proto_tree_add_subtree(
                            hiqnet_payload_tree, tvb, offset, -1, ett_hiqnet, NULL, "Interface");
                        proto_tree_add_item(hiqnet_ifaces_tree, hf_hiqnet_maxmsgsize, tvb, offset, 4, ENC_BIG_ENDIAN);
                        offset += 4;
                        offset = hiqnet_display_netinfo(hiqnet_ifaces_tree, tvb, offset);
                        ifacecount -= 1;
                    }
                }
                break;
            case HIQNET_REQADDR_MSG :
                /
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_devaddr, tvb, offset, 2, ENC_BIG_ENDIAN);
                break;
            case HIQNET_SETADDR_MSG :
                offset = hiqnet_display_sernum(hiqnet_payload_tree, tvb, offset);
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_newdevaddr, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                hiqnet_display_netinfo(hiqnet_payload_tree, tvb, offset);
                break;
            case HIQNET_SETATTR_MSG : /
                attrcount = tvb_get_ntohs(tvb, offset);
                proto_tree_add_item(hiqnet_payload_tree, hf_hiqnet_attrcount, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
                while (attrcount > 0) {
                    hiqnet_attribute_tree = proto_tree_add_subtree(
                        hiqnet_payload_tree, tvb, offset, -1, ett_hiqnet, NULL, "Attribute");
                    proto_tree_add_item(hiqnet_attribute_tree, hf_hiqnet_attrid, tvb, offset, 2, ENC_BIG_ENDIAN);
                    offset += 2;
                    offset = hiqnet_display_data(hiqnet_attribute_tree, tvb, offset);
                    attrcount -= 1;
                }
                break;
                /
            case HIQNET_RESERVED0_MSG:
            case HIQNET_RESERVED1_MSG:
                /
            case HIQNET_ADDRUSED_MSG:
                break;
            default : /
                /
                break;
        }
    }
    return tvb_reported_length(tvb);
}
