static int
dissect_amqp_0_9_method_connection_start(tvbuff_t *tvb, packet_info *pinfo,
    int offset, proto_tree *args_tree)
{
    proto_item *ti;

    /
    proto_tree_add_item(args_tree, hf_amqp_method_connection_start_version_major,
        tvb, offset, 1, ENC_BIG_ENDIAN);
    offset += 1;

    /
    proto_tree_add_item(args_tree, hf_amqp_method_connection_start_version_minor,
        tvb, offset, 1, ENC_BIG_ENDIAN);
    offset += 1;

    /
    ti = proto_tree_add_item(
        args_tree, hf_amqp_method_connection_start_server_properties,
        tvb, offset + 4, tvb_get_ntohl(tvb, offset), ENC_NA);
    dissect_amqp_0_9_field_table(tvb, pinfo, offset + 4, tvb_get_ntohl(tvb, offset), ti);
    offset += 4 + tvb_get_ntohl(tvb, offset);

    /
    proto_tree_add_item(args_tree, hf_amqp_0_9_method_connection_start_mechanisms,
        tvb, offset + 4, tvb_get_ntohl(tvb, offset), ENC_NA);
    offset += 4 + tvb_get_ntohl(tvb, offset);

    /
    proto_tree_add_item(args_tree, hf_amqp_0_9_method_connection_start_locales,
        tvb, offset + 4, tvb_get_ntohl(tvb, offset), ENC_NA);
    offset += 4 + tvb_get_ntohl(tvb, offset);

    return offset;
}
