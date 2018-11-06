static void
dissect_802154_h_ie_time_correction(tvbuff_t *tvb, proto_tree *h_inf_elem_tree, guint offset, packet_info *pinfo) {

    guint16     raw_data;
    proto_tree  *time_correction_tree = NULL;
    gboolean    is_nack;
    proto_item  *time_correction_item = NULL;
    proto_item  *boolean_item = NULL;
    proto_item  *drift_item = NULL;

    raw_data = tvb_get_letohs(tvb, offset);
    is_nack = (gboolean) ((raw_data & (guint16)0x8000) ? 1 : 0);
    gint16 drift_us = 0;

    if(raw_data & 0x0800) { /
        drift_us = (gint16) (raw_data | 0xf000);
    } else { /
        drift_us = (gint16) (raw_data & 0x0fff);
    }
    time_correction_item = proto_tree_add_bytes_format_value(h_inf_elem_tree, hf_ieee802154_time_correction, tvb, offset, 2, NULL,
                            "Time correction: %d us | Non-acknowledgement: %s", drift_us, (guint32) is_nack ? "True" : "False");
    time_correction_tree = proto_item_add_subtree(time_correction_item, ett_ieee802154_h_ie_payload);

    /
    if(raw_data <= 0x8fff) {
        drift_item = proto_tree_add_int(time_correction_tree, hf_ieee802154_time_correction_value, tvb, offset, 2, drift_us);
        PROTO_ITEM_SET_GENERATED(drift_item);
        boolean_item = proto_tree_add_boolean(time_correction_tree, hf_ieee802154_nack, tvb, offset, 2, (guint32) is_nack);
        PROTO_ITEM_SET_GENERATED(boolean_item);
    }
    /
    else {
        expert_add_info(pinfo, time_correction_item, &ei_ieee802154_time_correction_error);
    }
} /
