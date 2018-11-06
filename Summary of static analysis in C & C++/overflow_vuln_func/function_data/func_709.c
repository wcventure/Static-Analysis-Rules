static void
dissect_ieee802154_header_ie(tvbuff_t *tvb, packet_info *pinfo _U_, proto_tree *tree, guint *offset, ieee802154_packet *packet)
{
    proto_tree *subtree;
    proto_item *header_item;
    guint16     header_ie;
    guint16     id;
    guint16     length;
    guint       header_length;

    static const int * fields[] = {
        &hf_ieee802154_header_ie_type,
        &hf_ieee802154_header_ie_id,
        &hf_ieee802154_header_ie_length,
        NULL
    };

    header_item = proto_tree_add_item(tree, hf_ieee802154_header_ie, tvb, *offset, -1, ENC_NA);
    header_length = 0;

    do {
        header_ie =  tvb_get_letohs(tvb, *offset);
        id        = (guint16) ((header_ie & IEEE802154_HEADER_IE_ID_MASK) >> 7);
        length    = (guint16) (header_ie & IEEE802154_HEADER_IE_LENGTH_MASK);
        header_length += 2 + length;

        /
        if (length > 0) {
            subtree = proto_item_add_subtree(header_item, ett_ieee802154_header);
            proto_item_append_text(subtree, ", Element ID: %s, Length: %d", val_to_str_const(id, ieee802154_header_ie_names, "Unknown IE"), length);
            proto_tree_add_bitmask(subtree, tvb, *offset, hf_ieee802154_header_ie_tlv, ett_ieee802154_header_ie, fields, ENC_LITTLE_ENDIAN);
            *offset += 2;

            switch(id){

                case IEEE802154_HEADER_IE_TIME_CORR:
                    // 7.4.2.7 Time Correction IE
                    dissect_802154_h_ie_time_correction(tvb, subtree, *offset, pinfo);
                    break;

                case IEEE802154_HEADER_VENDOR_SPECIFIC:
                    // 7.4.2.2 Vendor Specific Header IE
                    // TODO

                case IEEE802154_HEADER_IE_CSL:
                    // 7.4.2.3 CSL IE
                    // TODO

                case IEEE802154_HEADER_IE_RIT:
                    // 7.4.2.4 RIT IE
                    // TODO

                case IEEE802154_HEADER_IE_DSME_PAN:
                    // 7.4.2.5 DSME PAN descriptor IE
                    // TODO

                case IEEE802154_HEADER_IE_RENDEZVOUS:
                    // 7.4.2.6 Rendezvous Time IE

                case IEEE802154_HEADER_IE_EXT_DSME_PAN:
                    // 7.4.2.8 Extended DSME PAN descriptor IE
                    // TODO

                case IEEE802154_HEADER_IE_FSCD:
                    // 7.4.2.9 Fragment Sequence Context Description (FSCD) IE
                    // TODO

                case IEEE802154_HEADER_IE_SMPL_SUPER_FRM:
                    // 7.4.2.10 Simplified Superframe Specification IE
                    // TODO

                case IEEE802154_HEADER_IE_SMPL_GTS:
                    // 7.4.2.11 Simplified GTS Specification IE
                    // TODO

                case IEEE802154_HEADER_IE_LECIM:
                    // 7.4.2.12 LECIM Capabilities IE
                    // TODO

                case IEEE802154_HEADER_IE_RCC_CAP:
                    // 7.4.2.13 RCC Capabilities IE
                    // TODO

                case IEEE802154_HEADER_IE_RCCN:
                    // 7.4.2.14 RCCN Descriptor IE
                    // TODO

                case IEEE802154_HEADER_IE_GLOBAL_TIME:
                    // 7.4.2.15 Global Time IE
                    // TODO

                case IEEE802154_HEADER_IE_DA_IE:
                    // 7.4.2.16 DA IE
                    // TODO

                case IEEE802154_HEADER_IE_TRLE:
                    // ???????
                    // TODO

                default:
                    expert_add_info(pinfo, header_item, &ei_ieee802154_unsupported_element_id);
                    break;
            }
            *offset += length;
        } else {
            // Header IE of empty length
            switch(id){
                case IEEE802154_HEADER_IE_EID_TERM1:
                    //header_item = proto_tree_add_item(tree, hf_ieee802154_header_ie, tvb, *offset, 2 + length, ENC_NA);
                    subtree = proto_item_add_subtree(header_item, ett_ieee802154_header);
                    proto_item_append_text(subtree, ", Element ID: %s, Length: %d", val_to_str_const(id, ieee802154_header_ie_names, "Unknown IE"), length);
                    proto_tree_add_bitmask(subtree, tvb, *offset, hf_ieee802154_header_termination, ett_ieee802154_header_ie, fields, ENC_LITTLE_ENDIAN);

                    *offset += 2;
                    break;

                case IEEE802154_HEADER_IE_EID_TERM2:
                    //header_item = proto_tree_add_item(tree, hf_ieee802154_header_ie, tvb, *offset, 2 + length, ENC_NA);
                    subtree = proto_item_add_subtree(header_item, ett_ieee802154_header);
                    proto_item_append_text(subtree, ", Element ID: %s, Length: %d", val_to_str_const(id, ieee802154_header_ie_names, "Unknown IE"), length);
                    proto_tree_add_bitmask(subtree, tvb, *offset, hf_ieee802154_header_termination, ett_ieee802154_header_ie, fields, ENC_LITTLE_ENDIAN);

                    *offset += 2;
                    break;

                default:
                    subtree = proto_item_add_subtree(header_item, ett_ieee802154_header);
                    proto_item_append_text(subtree, ", Element ID: %s, Length: %d", val_to_str_const(id, ieee802154_header_ie_names, "Unknown IE"), length);
                    proto_tree_add_bitmask(subtree, tvb, *offset, hf_ieee802154_header_ie_tlv, ett_ieee802154_header_ie, fields, ENC_LITTLE_ENDIAN);
                    *offset += 2;
                    expert_add_info(pinfo, header_item, &ei_ieee802154_unsupported_element_id);
                    break;
            }
        }
    } while ((tvb_reported_length_remaining(tvb, *offset) > IEEE802154_MIC_LENGTH(packet->security_level) + IEEE802154_FCS_LEN + 1) &&
             (id != IEEE802154_HEADER_IE_EID_TERM1) &&
             (id != IEEE802154_HEADER_IE_EID_TERM2));

    // Once the dissection is over, the length of the header is known
    proto_item_set_len(header_item, header_length);

    /
    if (id == IEEE802154_HEADER_IE_EID_TERM1) {
        packet->payload_ie_present = TRUE;
    }
    else {
        packet->payload_ie_present = FALSE;
    }

} /
