static void
dissect_ieee802154_header_ie(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, ieee802154_packet *packet)
{
    proto_item *ies_item;
    proto_tree *ies_tree;
    proto_item *subitem;
    proto_tree *volatile subtree;  // may change in TRY block and is used in CATCH
    tvbuff_t   *content;
    guint16     ie_header;
    guint16     id;
    guint16     length;
    guint16     consumed;
    const char *name;
    volatile guint total_length = 0;  // silence spurious -Wclobber warning

    ies_item = proto_tree_add_item(tree, hf_ieee802154_header_ies, tvb, *offset, -1, ENC_NA);
    ies_tree = proto_item_add_subtree(ies_item, ett_ieee802154_header_ies);

    do {
        ie_header =  tvb_get_letohs(tvb, *offset);
        id        = (guint16) ((ie_header & IEEE802154_HEADER_IE_ID_MASK) >> 7);
        length    = (guint16) (ie_header & IEEE802154_HEADER_IE_LENGTH_MASK);
        content   = tvb_new_subset_length(tvb, *offset + 2, length);
        name      = try_val_to_str(id, ieee802154_header_ie_names);
        total_length += 2 + length;

        subtree = NULL;  // for checking in catch block if already created

        TRY {  // primarily to catch out-of-bounds exceptions if a IE is expected to have more content
            switch (id) {
                case IEEE802154_HEADER_IE_HT1:
                    proto_item_append_text(ies_item, ", %s", name);
                    subtree = create_header_ie_tree(tvb, ies_tree, *offset, length, hf_ieee802154_hie_ht1, ett_ieee802154_hie_ht1, &subitem);
                    consumed = 0;
                    break;
                case IEEE802154_HEADER_IE_HT2:
                    proto_item_append_text(ies_item, ", %s", name);
                    subtree = create_header_ie_tree(tvb, ies_tree, *offset, length, hf_ieee802154_hie_ht2, ett_ieee802154_hie_ht2, &subitem);
                    consumed = 0;
                    break;
                case IEEE802154_HEADER_IE_TIME_CORR: // 7.4.2.7 Time Correction IE
                    proto_item_append_text(ies_item, ", %s", name);
                    subtree = create_header_ie_tree(tvb, ies_tree, *offset, length, hf_ieee802154_hie_time_correction, ett_ieee802154_hie_time_correction, &subitem);
                    consumed = dissect_802154_hie_time_correction(content, pinfo, subtree, packet, subitem, ies_item);
                    break;

                case IEEE802154_HEADER_IE_CSL: // 7.4.2.3 CSL IE
                    proto_item_append_text(ies_item, ", %s", name);
                    subtree = create_header_ie_tree(tvb, ies_tree, *offset, length, hf_ieee802154_hie_csl, ett_ieee802154_hie_csl, &subitem);
                    proto_tree_add_item(subtree, hf_ieee802154_hie_csl_phase, content, 0, 2, ENC_LITTLE_ENDIAN);
                    proto_tree_add_item(subtree, hf_ieee802154_hie_csl_period, content, 2, 2, ENC_LITTLE_ENDIAN);
                    consumed = 4;
                    if (length >= 6) {
                        proto_tree_add_item(subtree, hf_ieee802154_hie_csl_rendezvous_time, content, 4, 2, ENC_LITTLE_ENDIAN);
                        consumed += 2;
                    }
                    break;

                // the IEs defined in 802.15.4-2015 that are not yet supported
                case IEEE802154_HEADER_IE_RIT:  // TODO: 7.4.2.4 RIT IE
                case IEEE802154_HEADER_IE_DSME_PAN:  // TODO: 7.4.2.5 DSME PAN descriptor IE
                case IEEE802154_HEADER_IE_RENDEZVOUS:  // TODO: 7.4.2.6 Rendezvous Time IE
                case IEEE802154_HEADER_IE_EXT_DSME_PAN:  // TODO: 7.4.2.8 Extended DSME PAN descriptor IE
                case IEEE802154_HEADER_IE_FSCD:  // TODO: 7.4.2.9 Fragment Sequence Context Description (FSCD) IE
                case IEEE802154_HEADER_IE_SMPL_SUPER_FRM:  // TODO: 7.4.2.10 Simplified Superframe Specification IE
                case IEEE802154_HEADER_IE_SMPL_GTS:  // TODO: 7.4.2.11 Simplified GTS Specification IE
                case IEEE802154_HEADER_IE_LECIM:  // TODO: 7.4.2.12 LECIM Capabilities IE
                case IEEE802154_HEADER_IE_RCC_CAP:  // TODO: 7.4.2.13 RCC Capabilities IE
                case IEEE802154_HEADER_IE_RCCN:  // TODO: 7.4.2.14 RCCN Descriptor IE
                case IEEE802154_HEADER_IE_GLOBAL_TIME:  // TODO: 7.4.2.15 Global Time IE
                case IEEE802154_HEADER_IE_DA_IE:  // TODO: 7.4.2.16 DA IE
                case IEEE802154_HEADER_IE_TRLE:  // TODO: F5.1.1 TRLE Descriptor IE
                    proto_item_append_text(ies_item, ", Unsupported: %s", name);
                    subtree = create_header_ie_tree(tvb, ies_tree, *offset, length, hf_ieee802154_hie_unsupported, ett_ieee802154_hie_unsupported, &subitem);
                    proto_item_append_text(subitem, ": %s", name);
                    if (length > 0) {
                        proto_tree_add_item(subtree, hf_ieee802154_hie_unknown_content, content, 0, length, ENC_NA);
                    }
                    expert_add_info(pinfo, subitem, &ei_ieee802154_ie_unsupported_element_id);
                    consumed = length;
                    break;

                // unknown IE
                default:
                    proto_item_append_text(ies_item, ", Unknown IE 0x%02x", id);
                    subtree = create_header_ie_tree(tvb, ies_tree, *offset, length, hf_ieee802154_hie_unknown, ett_ieee802154_hie_unknown, &subitem);
                    proto_item_append_text(subitem, ": 0x%02x", id);
                    if (length > 0) {
                        proto_tree_add_item(subtree, hf_ieee802154_hie_unknown_content, content, 0, length, ENC_NA);
                    }
                    expert_add_info(pinfo, subitem, &ei_ieee802154_ie_unknown_element_id);
                    consumed = length;
                    break;
            }

            if (consumed < length) {
                proto_tree_add_item(subtree, hf_ieee802154_hie_unknown_content, content, consumed, length - consumed, ENC_NA);
                expert_add_info(pinfo, subitem, &ei_ieee802154_ie_unknown_extra_content);
            }
        }
        CATCH_ALL {
            if (subtree == NULL) {  // if not done yet, create a tree for the exception
                subtree = create_header_ie_tree(tvb, ies_tree, *offset, length, hf_ieee802154_hie_unknown, ett_ieee802154_hie_unknown, &subitem);
            }
            show_exception(tvb, pinfo, subtree, EXCEPT_CODE, GET_MESSAGE);
        }
        ENDTRY;

        *offset += 2 + length;
    } while ((tvb_reported_length_remaining(tvb, *offset) > IEEE802154_MIC_LENGTH(packet->security_level) + IEEE802154_FCS_LEN + 1) &&
             (id != IEEE802154_HEADER_IE_HT1) &&
             (id != IEEE802154_HEADER_IE_HT2));

    // Once the dissection is over, the length of the header is known
    proto_item_set_len(ies_item, total_length);

    // Presence of Payload IEs is determined by how the Header IEs are terminated
    if (id == IEEE802154_HEADER_IE_HT1) {
        packet->payload_ie_present = TRUE;
    }
    else {
        packet->payload_ie_present = FALSE;
    }
} /
