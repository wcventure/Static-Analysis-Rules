static gint
dissect_bip_application_parameters(tvbuff_t *tvb, packet_info *pinfo,
        proto_tree *tree, gint offset, gint parameters_length)
{
    proto_item  *item;
    proto_item  *parameter_item;
    proto_tree  *parameter_tree;
    gint         parameter_length;
    guint8       parameter_id;
    static gint  required_length_map[] = {0, 2, 2, 1, 4, 4, 4, 1, 1, 16, 1};

    while (parameters_length > 0) {
        parameter_id = tvb_get_guint8(tvb, offset);
        parameter_length = tvb_get_guint8(tvb, offset + 1);

        parameter_item = proto_tree_add_text(tree, tvb, offset, parameter_length + 2,
                "Parameter: %s", val_to_str_const(parameter_id,
                bip_application_parameters_vals, "Unknown"));
        parameter_tree = proto_item_add_subtree(parameter_item, ett_btobex_application_parameters);

        proto_tree_add_item(parameter_tree, hf_bip_application_parameter_id, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset += 1;

        proto_tree_add_item(parameter_tree, hf_application_parameter_length, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset += 1;

       if (parameter_id < sizeof(required_length_map) &&
                required_length_map[parameter_id] != parameter_length) {
            item = proto_tree_add_item(parameter_tree, hf_application_parameter_data, tvb, offset, parameter_length, ENC_BIG_ENDIAN);
            expert_add_info_format(pinfo, item, PI_PROTOCOL, PI_WARN,
                    "According to the specification this parameter length should be %i, but there is %i",
                    required_length_map[parameter_id], parameter_length);
        } else switch (parameter_id) {
            case 0x01:
                proto_tree_add_item(parameter_tree, hf_bip_application_parameter_data_number_of_returned_handles, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x02:
                proto_tree_add_item(parameter_tree, hf_bip_application_parameter_data_list_start_offset, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x03:
                proto_tree_add_item(parameter_tree, hf_bip_application_parameter_data_latest_captured_images, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x04:
                proto_tree_add_item(parameter_tree, hf_bip_application_parameter_data_partial_file_length, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x05:
                proto_tree_add_item(parameter_tree, hf_bip_application_parameter_data_partial_file_start_offset, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x06:
                proto_tree_add_item(parameter_tree, hf_bip_application_parameter_data_total_file_size, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x07:
                proto_tree_add_item(parameter_tree, hf_bip_application_parameter_data_end_flag, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x08:
                proto_tree_add_item(parameter_tree, hf_bip_application_parameter_data_remote_display, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x09:
                proto_tree_add_item(parameter_tree, hf_bip_application_parameter_data_service_id, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            case 0x0A:
                proto_tree_add_item(parameter_tree, hf_bip_application_parameter_data_store_flag, tvb, offset, required_length_map[parameter_id], ENC_BIG_ENDIAN);
                break;
            default:
                proto_tree_add_item(parameter_tree, hf_application_parameter_data, tvb, offset, parameter_length, ENC_BIG_ENDIAN);
        }

        parameters_length -= 2 + parameter_length;
        offset += parameter_length;
    }

    return offset;
}
