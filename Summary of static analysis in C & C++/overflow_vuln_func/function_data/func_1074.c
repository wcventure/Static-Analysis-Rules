static int
dissect_ieee802154_payload_ie(tvbuff_t *tvb, packet_info *pinfo _U_, proto_tree *tree, int offset)
{
    proto_tree *subtree;
    proto_item *payload_item;
    proto_tree *payload_tree;
    proto_item *mlme_item;
    proto_tree *mlme_subtree;
    proto_item *ietf_item;
    proto_tree *ietf_subtree;
    guint16     payload_ie;
    guint16     pie_id;
    int         pie_length;
    int         orig_offset;

    static const int * fields[] = {
        &hf_ieee802154_payload_ie_type,
        &hf_ieee802154_payload_ie_id,
        &hf_ieee802154_payload_ie_length,
        NULL
    };

    orig_offset = offset;

    payload_item = proto_tree_add_item(tree, hf_ieee802154_payload_ie, tvb, offset, -1, ENC_NA);
    payload_tree = proto_item_add_subtree(payload_item, ett_ieee802154_payload_ie);

    do {
        payload_ie    =  tvb_get_letohs(tvb, offset);
        pie_id        = (guint16) ((payload_ie & IEEE802154_PAYLOAD_IE_ID_MASK) >> 11);
        pie_length    =  payload_ie & IEEE802154_PAYLOAD_IE_LENGTH_MASK;

        switch (pie_id) {

            case IEEE802154_PAYLOAD_IE_MLME:
                // 7.4.3.2 MLME IE
                mlme_item = proto_tree_add_item(payload_tree, hf_ieee802154_mlme, tvb, offset, pie_length + 2, ENC_NA);
                mlme_subtree = proto_item_add_subtree(mlme_item, ett_ieee802154_mlme);
                proto_tree_add_bitmask(mlme_subtree, tvb, (const guint) offset, hf_ieee802154_payload_ie_tlv, ett_ieee802154_payload_ie, fields, ENC_LITTLE_ENDIAN);
                offset += 2;

                while (offset < (orig_offset + pie_length)) {
                    offset += dissect_ieee802154_payload_mlme_sub_ie(tvb, pinfo, mlme_subtree, offset);
                }
                break;

            case IEEE802154_PAYLOAD_IE_GID_TERM:
                // 7.4.3.3 Payload Termination IE
                subtree = proto_tree_add_subtree(payload_tree, tvb, offset, pie_length + 2, ett_ieee802154_payload, NULL, "Payload IE");
                proto_item_append_text(subtree, ", %s, Length: %d", val_to_str_const(pie_id, ieee802154_payload_ie_names, "Unknown IE"), pie_length);
                proto_tree_add_bitmask(subtree, tvb, (const guint) offset, hf_ieee802154_payload_ie_tlv, ett_ieee802154_payload_ie, fields, ENC_LITTLE_ENDIAN);
                offset += 2;
                break;

            case IEEE802154_PAYLOAD_IE_VENDOR:
                // 7.4.4.30 Vendor Specific Nested IE
                subtree = proto_tree_add_subtree(payload_tree, tvb, offset, pie_length + 2, ett_ieee802154_payload, NULL, "Payload IE");
                proto_item_append_text(subtree, ", %s, Length: %d", val_to_str_const(pie_id, ieee802154_payload_ie_names, "Unknown IE"), pie_length);
                proto_tree_add_bitmask(subtree, tvb, (const guint) offset, hf_ieee802154_payload_ie_tlv, ett_ieee802154_payload_ie, fields, ENC_LITTLE_ENDIAN);
                offset += 2;

                offset += dissect_ieee802154_vendor_ie(tvb, pinfo, subtree, offset, pie_length);
                break;

            case IEEE802154_PAYLOAD_IE_IETF:
                ietf_item = proto_tree_add_item(payload_tree, hf_ieee802154_payload_ie_ietf, tvb, offset, pie_length + 2, ENC_NA);
                ietf_subtree = proto_item_add_subtree(ietf_item, ett_ieee802154_payload_ie_ietf);
                proto_item_append_text(ietf_item, ", Length: %d", pie_length);
                proto_tree_add_bitmask(ietf_subtree, tvb, (const guint) offset, hf_ieee802154_payload_ie_tlv, ett_ieee802154_payload_ie, fields, ENC_LITTLE_ENDIAN);
                offset += 2;

                offset += dissect_ieee802154_6top(tvb, pinfo, ietf_subtree, (guint) offset, pie_length);
                break;

            default:
                subtree = proto_tree_add_subtree(payload_tree, tvb, offset, pie_length + 2, ett_ieee802154_payload, NULL, "Payload IE");
                proto_item_append_text(subtree, ", %s, Length: %d", val_to_str_const(pie_id, ieee802154_payload_ie_names, "Unknown IE"), pie_length);
                proto_tree_add_bitmask(subtree, tvb, (const guint) offset, hf_ieee802154_payload_ie_tlv, ett_ieee802154_payload_ie, fields, ENC_LITTLE_ENDIAN);
                offset += 2;

                if (pie_length > 0) {
                    proto_tree_add_item(subtree, hf_ieee802154_payload_ie_data, tvb, offset, pie_length, ENC_NA);
                    offset += pie_length;
                }
        }
    } while ((tvb_reported_length_remaining(tvb, offset) > 1)
             && (pie_id != IEEE802154_PAYLOAD_IE_GID_TERM));

    // Once the dissection is over, the length of the header is known
    proto_item_set_len(payload_item, offset - orig_offset);

    return (offset - orig_offset);
}
