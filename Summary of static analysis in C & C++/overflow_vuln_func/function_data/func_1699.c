static void dissect_transport_info_response(tvbuff_t *tvb, proto_tree *tree,
                                            guint offset, guint length_remaining)
{
    char *strange_string;

    /
    proto_tree_add_item(tree, hf_msmms_command_prefix1_command_level, tvb, offset, 4, ENC_LITTLE_ENDIAN);
    offset += 4;
    proto_tree_add_item(tree, hf_msmms_command_prefix2, tvb, offset, 4, ENC_LITTLE_ENDIAN);
    offset += 4;

    /
    proto_tree_add_item(tree, hf_msmms_command_number_of_words, tvb, offset, 4, ENC_LITTLE_ENDIAN);
    offset += 4;

    /
    strange_string = tvb_get_ephemeral_unicode_string(tvb, offset, length_remaining - 12, ENC_LITTLE_ENDIAN);

    proto_tree_add_string(tree, hf_msmms_command_strange_string, tvb,
                          offset, length_remaining-12,
                          strange_string);
}
