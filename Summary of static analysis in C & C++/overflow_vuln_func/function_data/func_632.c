static gint
dissect_eir_ad_data(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    proto_item  *entry_item;
    proto_tree  *entry_tree;
    proto_item  *sub_item;
    gint         offset = 0;
    guint8       length;
    guint8       type;
    guint8       data_size;
    gint         end_offset;
    guint        i_uuid;

    data_size = tvb_length(tvb);

    while (offset < data_size) {
        length = tvb_get_guint8(tvb, offset);
        if (length <= 0) break;

        type = tvb_get_guint8(tvb, offset + 1);

        entry_item = proto_tree_add_none_format(tree, hf_btcommon_eir_ad_entry, tvb, offset, length + 1, "%s",
                val_to_str_const(type, bthci_cmd_eir_data_type_vals, "Unknown"));
        entry_tree = proto_item_add_subtree(entry_item, ett_eir_ad_entry);

        proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_length, tvb, offset, 1, ENC_NA);
        offset += 1;

        proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_type, tvb, offset, 1, ENC_NA);
        offset += 1;
        length -= 1;

        switch (type) {
        case 0x01: /
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_flags_reserved, tvb, offset, 1, ENC_NA);
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_flags_le_bredr_support_host, tvb, offset, 1, ENC_NA);
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_flags_le_bredr_support_controller, tvb, offset, 1, ENC_NA);
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_flags_bredr_not_support, tvb, offset, 1, ENC_NA);
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_flags_le_general_discoverable_mode, tvb, offset, 1, ENC_NA);
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_flags_le_limited_discoverable_mode, tvb, offset, 1, ENC_NA);
            offset += 1;

            break;
        case 0x02: /
        case 0x03: /
        case 0x14: /
        case 0x16: /
            end_offset = offset + length;
            while (offset < end_offset) {
                proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_uuid_16, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;
            }

            break;
        case 0x04: /
        case 0x05: /
        case 0x1F: /
        case 0x20: /
            end_offset = offset + length;
            while (offset < end_offset) {
                if (tvb_get_ntohs(tvb, offset) == 0x0000) {
                    sub_item = proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_uuid_32, tvb, offset, 4, ENC_BIG_ENDIAN);
                    proto_item_append_text(sub_item, " (%s)", val_to_str_ext_const(tvb_get_ntohs(tvb, offset + 2), &bt_sig_uuid_vals_ext, "Unknown"));
                } else {
                    sub_item = proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_custom_uuid, tvb, offset, 4, ENC_NA);

                    i_uuid = 0;
                    while (custom_uuid[i_uuid].name) {
                        if (custom_uuid[i_uuid].size != 4) {
                            i_uuid += 1;
                            continue;
                        }

                        if (tvb_memeql(tvb, offset, custom_uuid[i_uuid].uuid, 4) == 0) {
                            proto_item_append_text(sub_item, " (%s)", custom_uuid[i_uuid].name);
                            break;
                        }

                        i_uuid += 1;
                    }
                }

                offset += 4;
            }

            break;
        case 0x06: /
        case 0x07: /
        case 0x15: /
        case 0x21: /
            end_offset = offset + length;
            while (offset < end_offset) {
                if (tvb_get_ntohs(tvb, offset) == 0x0000 &&
                        tvb_get_ntohl(tvb, offset + 4) == 0x1000 &&
                        tvb_get_ntoh64(tvb, offset + 8) == G_GUINT64_CONSTANT(0x800000805F9B34FB)) {
                    sub_item = proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_uuid_128, tvb, offset, 16, ENC_NA);
                    proto_item_append_text(sub_item, " (%s)", val_to_str_ext_const(tvb_get_ntohs(tvb, offset + 2), &bt_sig_uuid_vals_ext, "Unknown"));
                } else {
                    sub_item = proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_custom_uuid, tvb, offset, 16, ENC_NA);

                    i_uuid = 0;
                    while (custom_uuid[i_uuid].name) {
                        if (custom_uuid[i_uuid].size != 16) {
                            i_uuid += 1;
                            continue;
                        }

                        if (tvb_memeql(tvb, offset, custom_uuid[i_uuid].uuid, 16) == 0) {
                            proto_item_append_text(sub_item, " (%s)", custom_uuid[i_uuid].name);
                            break;
                        }

                        i_uuid += 1;
                    }
                }

                offset += 16;
            }

            break;

        case 0x08: /
        case 0x09: /
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_name, tvb, offset, length, ENC_ASCII | ENC_NA);
            proto_item_append_text(entry_item, ": %s", tvb_format_text(tvb,offset, length));
            offset += length;

            break;
        case 0x10: /
            if (length == 16) { /
                sub_item = proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_data, tvb, offset, 16, ENC_NA);
                expert_add_info(pinfo, sub_item, &ei_eir_ad_undecoded);
            } else if (length == 8) { /
                guint16       vendor_id_source;
                guint16       vendor_id;
                guint16       product_id;
                const gchar  *str_val;

                proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_did_vendor_id_source, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                vendor_id_source = tvb_get_letohs(tvb, offset);
                offset += 2;

                if (vendor_id_source == DID_VENDOR_ID_SOURCE_BLUETOOTH_SIG) {
                    proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_did_vendor_id_bluetooth_sig, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                } else if (vendor_id_source == DID_VENDOR_ID_SOURCE_USB_FORUM) {
                    proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_did_vendor_id_usb_forum, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                } else {
                    proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_did_vendor_id, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                }
                vendor_id = tvb_get_letohs(tvb, offset);
                offset += 2;

                sub_item = proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_did_product_id, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                product_id = tvb_get_letohs(tvb, offset);
                offset += 2;

                if (vendor_id_source == DID_VENDOR_ID_SOURCE_USB_FORUM) {
                    str_val = val_to_str_ext_const(vendor_id << 16 | product_id, &ext_usb_products_vals, "Unknown");
                    proto_item_append_text(sub_item, " (%s)", str_val);
                }

                proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_did_version, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                offset += 2;
            } else {
                sub_item = proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_data, tvb, offset, length, ENC_NA);
                expert_add_info(pinfo, sub_item, &ei_command_unknown);
            }

            break;
        case 0x0A: /
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_tx_power, tvb, offset, 1, ENC_NA);
            offset += 1;

            break;
        case 0x0B: /
            /
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_ssp_oob_length, tvb, offset, 2, ENC_LITTLE_ENDIAN);
            offset += 2;

            break;
        case 0x0C: /
            /
            offset = dissect_bd_addr(hf_btcommon_eir_ad_bd_addr, tree, tvb, offset);

            break;

        case 0x0D: /
            call_dissector(btcommon_cod_handle, tvb_new_subset_length(tvb, offset, 3), pinfo, entry_tree);
            offset += 3;

            break;
        case 0x0E: /
        case 0x1D: /
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_hash_c, tvb, offset, 16, ENC_NA);
            offset += 16;

            break;
        case 0x0F: /
        case 0x1E: /
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_randomizer_r, tvb, offset, 16, ENC_NA);
            offset += 16;

            break;
        case 0x11: /
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_oob_flags_reserved, tvb, offset, 1, ENC_NA);
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_oob_flags_address_type, tvb, offset, 1, ENC_NA);
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_oob_flags_le_bredr_support, tvb, offset, 1, ENC_NA);
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_oob_flags_le_supported_host, tvb, offset, 1, ENC_NA);
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_oob_flags_data_present, tvb, offset, 1, ENC_NA);
            offset += 1;

            break;
        case 0x12: /
            sub_item = proto_tree_add_item(tree, hf_btcommon_eir_ad_connection_interval_min, tvb, offset, 2, ENC_LITTLE_ENDIAN);
            proto_item_append_text(sub_item, " (%g msec)",  tvb_get_letohs(tvb, offset) * 1.25);
            offset += 2;

            sub_item = proto_tree_add_item(tree, hf_btcommon_eir_ad_connection_interval_max, tvb, offset, 2, ENC_LITTLE_ENDIAN);
            proto_item_append_text(sub_item, " (%g msec)",  tvb_get_letohs(tvb, offset) * 1.25);
            offset += 2;

            proto_item_append_text(entry_item, ": %g - %g msec", tvb_get_letohs(tvb, offset - 4) * 1.25, tvb_get_letohs(tvb, offset - 2) * 1.25);

            break;
        case 0x17: /
        case 0x18: /
            end_offset = offset + length;
            while (offset < end_offset) {
                offset = dissect_bd_addr(hf_btcommon_eir_ad_bd_addr, entry_tree, tvb, offset);
            }

            break;
        case 0x19: /
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_appearance, tvb, offset, 2, ENC_LITTLE_ENDIAN);
            proto_item_append_text(entry_item,": %s", val_to_str(tvb_get_letohs(tvb, offset), bthci_cmd_appearance_vals, "Unknown"));
            offset += 2;

            break;
        case 0x1A: /
            /
            sub_item = proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_advertising_interval, tvb, offset, 2, ENC_LITTLE_ENDIAN);
            proto_item_append_text(sub_item, " (%g msec)", tvb_get_letohs(tvb, offset) * 0.625);
            offset += 2;

            break;
        case 0x1B: /
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_le_bd_addr_reserved, tvb, offset, 1, ENC_NA);
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_le_bd_addr_type, tvb, offset, 1, ENC_NA);
            offset += 1;

            offset = dissect_bd_addr(hf_btcommon_eir_ad_bd_addr, entry_tree, tvb, offset);

            break;
        case 0x1C: /
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_le_role, tvb, offset, 1, ENC_NA);
            offset += 1;

            break;
        case 0x3D: /
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_3ds_factory_test_mode, tvb, offset, 1, ENC_NA);
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_3ds_reserved, tvb, offset, 1, ENC_NA);
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_3ds_send_battery_level_report_on_startup, tvb, offset, 1, ENC_NA);
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_3ds_battery_level_reporting, tvb, offset, 1, ENC_NA);
            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_3ds_association_notification, tvb, offset, 1, ENC_NA);
            offset += 1;

            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_3ds_path_loss_threshold, tvb, offset, 1, ENC_NA);
            offset += 1;

            break;
        case 0xFF: / {
            guint16  company_id;

            proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_company_id, tvb, offset, 2, ENC_LITTLE_ENDIAN);
            company_id = tvb_get_letohs(tvb, offset);
            offset += 2;
            length -= 2;

            if (company_id == 0x000F && tvb_get_guint8(tvb, offset) == 0) { /
                proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_3ds_legacy_fixed, tvb, offset, 1, ENC_NA);
                offset += 1;

                proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_3ds_legacy_test_mode, tvb, offset, 1, ENC_NA);
                proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_3ds_legacy_fixed_6, tvb, offset, 1, ENC_NA);
                proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_3ds_legacy_ignored_5, tvb, offset, 1, ENC_NA);
                proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_3ds_legacy_fixed_4, tvb, offset, 1, ENC_NA);
                proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_3ds_legacy_ignored_1_3, tvb, offset, 1, ENC_NA);
                proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_3ds_legacy_3d_capable_tv, tvb, offset, 1, ENC_NA);
                offset += 1;

                proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_3ds_legacy_path_loss_threshold, tvb, offset, 1, ENC_NA);
                offset += 1;
            } else {
                sub_item = proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_data, tvb, offset, length, ENC_NA);
                expert_add_info(pinfo, sub_item, &ei_eir_ad_undecoded);
                offset += length;
            }
            }
            break;
        default:
            sub_item = proto_tree_add_item(entry_tree, hf_btcommon_eir_ad_data, tvb, offset, length, ENC_NA);
            expert_add_info(pinfo, sub_item, &ei_eir_ad_unknown);
            offset += length;
        }
    }

    if (tvb_length_remaining(tvb, offset) > 0) {
        proto_tree_add_item(tree, hf_btcommon_eir_ad_unused, tvb, offset, -1, ENC_NA);
        offset = tvb_length(tvb);
    }

    return offset + data_size;
}
