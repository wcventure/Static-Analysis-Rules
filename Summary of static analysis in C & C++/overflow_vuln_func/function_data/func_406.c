static gint
dissect_pbap_application_parameters(tvbuff_t *tvb, packet_info *pinfo,
        proto_tree *tree, gint offset, gint parameters_length)
{
    proto_item  *item;
    proto_item  *parameter_item;
    proto_tree  *parameter_tree;
    gint         parameter_length;
    guint8       parameter_id;
    static gint  required_length_map[] = {0, 1, -1, 1, 2, 2, 8, 1, 2, 1};

    while (parameters_length > 0) {
        parameter_id = tvb_get_guint8(tvb, offset);
        parameter_length = tvb_get_guint8(tvb, offset + 1);

        parameter_item = proto_tree_add_text(tree, tvb, offset, parameter_length + 2,
                "Parameter: %s", val_to_str_const(parameter_id,
                pbap_application_parameters_vals, "Unknown"));
        parameter_tree = proto_item_add_subtree(parameter_item, ett_btobex_application_parameters);

        proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_id, tvb, offset, 1, ENC_BIG_ENDIAN);
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
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_order, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x02:
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_search_value, tvb, offset, parameter_length, ENC_BIG_ENDIAN);
                break;
            case 0x03:
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_search_attribute, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x04:
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_max_list_count, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x05:
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_list_start_offset, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x06:
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_version, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_fn, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_n, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_photo, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_birthday, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_adr, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_label, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_tel, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_email, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_mailer, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_time_zone, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_geographic_position, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_title, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_role, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_logo, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_agent, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_name_of_organization, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_comments, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_revision, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_pronunciation_of_name, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_url, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_uid, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_key, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_nickname, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_categories, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_product_id, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_class, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_sort_string, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_timestamp, tvb, offset, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_reserved_29_31, tvb, offset, 4, ENC_BIG_ENDIAN);

                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_reserved_32_38, tvb, offset + 4, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_proprietary_filter, tvb, offset + 4, 4, ENC_BIG_ENDIAN);
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_filter_reserved_for_proprietary_filter_usage, tvb, offset + 4, 4, ENC_BIG_ENDIAN);
                break;
            case 0x07:
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_format, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x08:
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_phonebook_size, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x09:
                proto_tree_add_item(parameter_tree, hf_pbap_application_parameter_data_new_missed_calls, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            default:
                proto_tree_add_item(parameter_tree, hf_application_parameter_data, tvb, offset, parameter_length, ENC_BIG_ENDIAN);
        }

        parameters_length -= 2 + parameter_length;
        offset += parameter_length;
    }

    return offset;
}
