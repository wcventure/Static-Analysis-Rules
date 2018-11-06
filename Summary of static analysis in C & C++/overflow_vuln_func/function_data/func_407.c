static int
dissect_map_application_parameters(tvbuff_t *tvb, packet_info *pinfo,
        proto_tree *tree, gint offset, gint parameters_length)
{
    proto_item  *item;
    proto_item  *parameter_item;
    proto_tree  *parameter_tree;
    gint         parameter_length;
    guint8       parameter_id;
    static gint  required_length_map[] = {0, 2, 2, 1, -1, -1, 1, -1, -1, 1, 1, 1, 1, 1, 1, 1, 4, 2, 2, 1, 1, 1, 1, 1, 1, -1};

    while (parameters_length > 0) {
        parameter_id = tvb_get_guint8(tvb, offset);
        parameter_length = tvb_get_guint8(tvb, offset + 1);

        parameter_item = proto_tree_add_text(tree, tvb, offset, parameter_length + 2,
                "Parameter: %s", val_to_str_const(parameter_id,
                map_application_parameters_vals, "Unknown"));
        parameter_tree = proto_item_add_subtree(parameter_item, ett_btobex_application_parameters);

        proto_tree_add_item(parameter_tree, hf_map_application_parameter_id, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset += 1;

        proto_tree_add_item(parameter_tree, hf_application_parameter_length, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset += 1;

        if (parameter_id < sizeof(required_length_map) &&
                required_length_map[parameter_id] != -1 &&
                required_length_map[parameter_id] != parameter_length) {
            item = proto_tree_add_item(parameter_tree, hf_application_parameter_data, tvb, offset, parameter_length, ENC_BIG_ENDIAN);
            expert_add_info_format(pinfo, item, PI_PROTOCOL, PI_WARN,
                    "According to the specification this parameter length should be %i, but there is %i",
                    required_length_map[parameter_id], parameter_length);
        } else switch (parameter_id) {
            case 0x01:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_max_list_count, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x02:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_start_offset, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x03:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_filter_message_type_reserved, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_filter_message_type_mms,      tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_filter_message_type_email,    tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_filter_message_type_sms_cdma, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_filter_message_type_sms_gsm,  tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x04:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_filter_period_begin, tvb, offset, parameter_length, ENC_NA);
                break;
            case 0x05:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_filter_period_end, tvb, offset, parameter_length, ENC_BIG_ENDIAN);
                break;
            case 0x06:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_filter_read_status_reserved_6, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_filter_read_status_get_read, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_filter_read_status_get_unread, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x07:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_filter_recipient, tvb, offset, parameter_length, ENC_BIG_ENDIAN);
                break;
            case 0x08:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_filter_originator, tvb, offset, parameter_length, ENC_BIG_ENDIAN);
                break;
            case 0x09:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_filter_priority_reserved_6, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_filter_priority_get_high, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_filter_priority_non_high, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x0A:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_reserved_7, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_attachment, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x0B:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_reserved_7, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_transparent, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x0C:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_reserved_7, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_retry, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x0D:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_reserved_7, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_new_message, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x0E:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_reserved_7, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_notification_status, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x0F:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_mas_instance_id, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x10:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_reserved,             tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_reply_to_addressing,  tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_protected,            tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_sent,                 tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_read,                 tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_priority,             tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_attachment_size,      tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_text,                 tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_reception_status,     tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_size,                 tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_type,                 tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_recipient_addressing, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_recipient_name,       tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_sender_addressing,    tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_sender_name,          tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_datetime,             tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_parameter_mask_subject,              tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);

                break;
            case 0x11:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_folder_listing_size, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                 break;
            case 0x12:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_messages_listing_size, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x13:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_subject_length, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x14:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_reserved_7, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_charset, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x15:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_reserved_7, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_fraction_request, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x16:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_reserved_7, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_fraction_deliver, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x17:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_reserved_7, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_status_indicator, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x18:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_reserved_7, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_status_value, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x19:
                proto_tree_add_item(parameter_tree, hf_map_application_parameter_data_mse_time, tvb, offset, parameter_length, ENC_BIG_ENDIAN);
                break;
            default:
                proto_tree_add_item(parameter_tree, hf_application_parameter_data, tvb, offset, parameter_length, ENC_BIG_ENDIAN);
        }

        parameters_length -= 2 + parameter_length;
        offset += parameter_length;
    }

   return offset;
}
