static void
dissect_fcels_prlilo_payload (tvbuff_t *tvb, packet_info *pinfo _U_,
                              guint8 isreq, proto_item *ti, guint8 opcode)
{
    int offset = 0,
        stroff = 0;
    guint8 type;
    proto_tree *prli_tree, *svcpg_tree;
    int num_svcpg, payload_len, i, flag;
    proto_item *subti;
    gchar flagstr[100];

    /
    prli_tree = proto_item_add_subtree (ti, ett_fcels_prli);

    proto_tree_add_item (prli_tree, hf_fcels_opcode, tvb, offset, 1, FALSE);

    proto_tree_add_text (prli_tree, tvb, offset+1, 1,
                         "Page Length: %u",
                         tvb_get_guint8 (tvb, offset+1));
    payload_len = tvb_get_ntohs (tvb, offset+2);
    proto_tree_add_text (prli_tree, tvb, offset+2, 2,
                         "Payload Length: %u", payload_len);
    num_svcpg = payload_len/16;

    offset = 4;
    for (i = 0; i < num_svcpg; i++) {
        subti = proto_tree_add_text (prli_tree, tvb, offset, 16,
                                     "Service Parameter Page %u", i);
        svcpg_tree = proto_item_add_subtree (subti, ett_fcels_prli_svcpg);

        type = tvb_get_guint8 (tvb, offset);
        proto_tree_add_text (svcpg_tree, tvb, offset, 1,
                             "TYPE: %s",
                             val_to_str (type,
                                         fc_prli_fc4_val, "0x%x"));
        proto_tree_add_text (svcpg_tree, tvb, offset+1, 1,
                             "TYPE Code Extension: %u",
                             tvb_get_guint8 (tvb, offset+1));

        flag = tvb_get_guint8 (tvb, offset+2);
        flagstr[0] = '\0';
        stroff = 0;
        if (opcode != FC_ELS_TPRLO) {
            if (flag & 0x80) {
                strcpy (flagstr, "Orig PA Valid, ");
                stroff += 15;
            }
            if (flag & 0x40) {
                strcpy (&flagstr[stroff], "Resp PA Valid, ");
                stroff += 15;
            }

            if (opcode == FC_ELS_PRLI) {
                if (!isreq) {
                    if (flag & 0x20) {
                        strcpy (&flagstr[stroff], "Image Pair Estd., ");
                        stroff += 18;
                    }
                    else {
                        strcpy (&flagstr[stroff], "Image Pair Not Estd., ");
                        stroff += 22;
                    }
                }
                else {
                    if (flag & 0x20) {
                        strcpy (&flagstr[stroff], "Est Image Pair & Exchg Svc Param, ");
                        stroff += 34;
                    }
                    else {
                        strcpy (&flagstr[stroff], "Exchange Svc Param Only, ");
                        stroff += 25;
                    }
                }
            }
        }
        else { /
            if (flag & 0x80) {
                strcpy (flagstr, "3rd Party Orig PA Valid, ");
                stroff += 25;
            }
            if (flag & 0x40) {
                strcpy (&flagstr[stroff], "Resp PA Valid, ");
                stroff += 15;
            }
            if (flag & 0x20) {
                strcpy (&flagstr[stroff], "3rd Party N_Port Valid, ");
                stroff += 24;
            }
            if (flag & 0x10) {
                strcpy (&flagstr[stroff], "Global PRLO, ");
                stroff += 13;
            }
        }

        proto_tree_add_text (svcpg_tree, tvb, offset+2, 1,
                             "Flags: %s", flagstr);
        if (!isreq && (opcode != FC_ELS_TPRLO)) {
            /
            proto_tree_add_text (svcpg_tree, tvb, offset+2, 1,
                                 "Response Code: 0x%x",
                                 (tvb_get_guint8 (tvb, offset+2) & 0x0F));
        }
        if (opcode != FC_ELS_TPRLO) {
            proto_tree_add_text (svcpg_tree, tvb, offset+4, 4,
                                 "Originator PA: 0x%x",
                                 tvb_get_ntohl (tvb, offset+4));
        }
        else {
            proto_tree_add_text (svcpg_tree, tvb, offset+4, 4,
                                 "3rd Party Originator PA: 0x%x",
                                 tvb_get_ntohl (tvb, offset+4));
        }
        proto_tree_add_text (svcpg_tree, tvb, offset+8, 4,
                             "Responder PA: 0x%x",
                             tvb_get_ntohl (tvb, offset+8));

        if (type == FC_TYPE_SCSI) {
            flag = tvb_get_ntohs (tvb, offset+14);
            flagstr[0] = '\0';
            stroff = 0;
            
            if (flag & 0x2000) {
                if (isreq) {
                    strcpy (flagstr, "Task Retry Ident Req, ");
                    stroff += 22;
                }
                else {
                    strcpy (flagstr, "Task Retry Ident Acc, ");
                    stroff += 22;
                }
            }
            if (flag & 0x1000) {
                strcpy (&flagstr[stroff], "Retry Possible, ");
                stroff += 16;
            }
            if (flag & 0x0080) {
                strcpy (&flagstr[stroff], "Confirmed Comp, ");
                stroff += 16;
            }
            if (flag & 0x0040) {
                strcpy (&flagstr[stroff], "Data Overlay, ");
                stroff += 14;
            }
            if (flag & 0x0020) {
                strcpy (&flagstr[stroff], "Initiator, ");
                stroff += 11;
            }
            if (flag & 0x0010) {
                strcpy (&flagstr[stroff], "Target, ");
                stroff += 8;
            }
            if (flag & 0x0002) {
                strcpy (&flagstr[stroff], "Rd Xfer_Rdy Dis, ");
                stroff += 17;
            }
            if (flag & 0x0001) {
                strcpy (&flagstr[stroff], "Wr Xfer_Rdy Dis");
                stroff += 15;
            }
            proto_tree_add_text (svcpg_tree, tvb, offset+12, 4,
                                 "FCP Flags: 0x%x (%s)", flag,
                                 flagstr);
        }
        else if ((opcode == FC_ELS_PRLI) && !isreq) {
            proto_tree_add_text (svcpg_tree, tvb, offset+12, 4,
                                 "Service Parameter Response: 0x%x",
                                 tvb_get_ntohl (tvb, offset+12));
        }
        else if (opcode == FC_ELS_TPRLO) {
            proto_tree_add_text (svcpg_tree, tvb, offset+13, 3,
                                 "3rd Party N_Port Id: %s",
                                 fc_to_str (tvb_get_ptr (tvb, offset+13, 3)));
        }
    }
}
