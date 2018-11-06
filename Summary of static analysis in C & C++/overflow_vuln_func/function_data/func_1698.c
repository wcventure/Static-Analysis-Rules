static void dissect_request_server_file(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
                                        guint offset, guint length_remaining)
{
    char *server_file;

    /
    proto_tree_add_item(tree, hf_msmms_command_prefix1_command_level, tvb, offset, 4, ENC_LITTLE_ENDIAN);
    offset += 4;
    proto_tree_add_item(tree, hf_msmms_command_prefix2, tvb, offset, 4, ENC_LITTLE_ENDIAN);
    offset += 4;

    offset += 4;
    offset += 4;

    /
    server_file = tvb_get_ephemeral_unicode_string(tvb, offset, length_remaining - 16, ENC_LITTLE_ENDIAN);

    proto_tree_add_string(tree, hf_msmms_command_server_file, tvb,
                          offset, length_remaining-16,
                          server_file);

    col_append_fstr(pinfo->cinfo, COL_INFO, " (%s)",
                    format_text((guchar*)server_file, (length_remaining - 16)/2));
}
