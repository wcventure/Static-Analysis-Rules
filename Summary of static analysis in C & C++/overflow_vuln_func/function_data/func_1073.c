static int
dissect_ieee802154_vendor_ie(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, gint offset, gint pie_length)
{
    tvbuff_t  *next_tvb;
    guint32    vendor_oui;

    vendor_oui = tvb_get_letoh24(tvb, offset);
    proto_item_append_text(tree, ", Vendor OUI: %06X (%s)", vendor_oui,
            val_to_str_const(vendor_oui, ieee802154_vendor_oui_names, "unknown"));
    proto_tree_add_uint_format_value(tree, hf_ieee802154_payload_ie_vendor_oui, tvb, offset, 3,
            vendor_oui, "%06X (%s)", vendor_oui, val_to_str_const(vendor_oui, ieee802154_vendor_oui_names, "unknown"));
    offset += 3; /
    pie_length -= 3;
    next_tvb = tvb_new_subset_length(tvb, offset, pie_length);

    switch (vendor_oui) {

        case IEEE802154_VENDOR_OUI_ZIGBEE:
            call_dissector_with_data(zigbee_ie_handle, next_tvb, pinfo, tree, &pie_length);
            break;

        default:
            call_data_dissector(next_tvb, pinfo, tree);
            break;
    }

    return pie_length + 3;
}
