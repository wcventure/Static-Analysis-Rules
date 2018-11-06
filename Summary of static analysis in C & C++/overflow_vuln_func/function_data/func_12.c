static void dissect_server_info(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
                                guint offset)
{
    guint32 server_version_length;
    guint32 tool_version_length;
    guint32 download_update_player_length;
    guint32 password_encryption_type_length;
    char    *server_version;
    char    *tool_version;
    char    *download_update_player;
    char    *password_encryption_type;

    /
    proto_tree_add_item(tree, hf_msmms_command_prefix1_error, tvb, offset, 4, ENC_LITTLE_ENDIAN);
    offset += 4;
    proto_tree_add_item(tree, hf_msmms_command_prefix2, tvb, offset, 4, ENC_LITTLE_ENDIAN);
    offset += 4;

    /
    offset += 4;
    offset += 4;
    offset += 4;
    offset += 4;
    offset += 4;
    offset += 4;
    offset += 4;
    offset += 4;


    /
    server_version_length = tvb_get_letohl(tvb, offset);
    proto_tree_add_item(tree, hf_msmms_command_server_version_length, tvb, offset, 4, ENC_LITTLE_ENDIAN);
    offset += 4;

    /
    tool_version_length = tvb_get_letohl(tvb, offset);
    proto_tree_add_item(tree, hf_msmms_command_tool_version_length, tvb, offset, 4, ENC_LITTLE_ENDIAN);
    offset += 4;

    /
    download_update_player_length = tvb_get_letohl(tvb, offset);
    proto_tree_add_item(tree, hf_msmms_command_update_url_length, tvb, offset, 4, ENC_LITTLE_ENDIAN);
    offset += 4;

    /
    password_encryption_type_length = tvb_get_letohl(tvb, offset);
    proto_tree_add_item(tree, hf_msmms_command_password_type_length, tvb, offset, 4, ENC_LITTLE_ENDIAN);
    offset += 4;

    /
    if (server_version_length > 1)
    {
        server_version = tvb_get_ephemeral_unicode_string(tvb, offset, server_version_length*2, ENC_LITTLE_ENDIAN);

        /
        proto_tree_add_string(tree, hf_msmms_command_server_version, tvb,
                              offset, server_version_length*2,
                              server_version);

        col_append_fstr(pinfo->cinfo, COL_INFO, " (version='%s')",
                    format_text((guchar*)server_version, server_version_length));
    }
    offset += (server_version_length*2);


    /
    if (tool_version_length > 1)
    {
        tool_version = tvb_get_ephemeral_unicode_string(tvb, offset, tool_version_length*2, ENC_LITTLE_ENDIAN);

        /
        proto_tree_add_string(tree, hf_msmms_command_tool_version, tvb,
                              offset, tool_version_length*2,
                              format_text((guchar*)tool_version, tool_version_length));
    }
    offset += (tool_version_length*2);

    /
    if (download_update_player_length > 1)
    {
        download_update_player = tvb_get_ephemeral_unicode_string(tvb, offset, download_update_player_length*2, ENC_LITTLE_ENDIAN);

        proto_tree_add_string(tree, hf_msmms_command_update_url, tvb,
                              offset, download_update_player_length*2,
                              download_update_player);
    }
    offset += (download_update_player_length*2);

    /
    if (password_encryption_type_length > 1)
    {
        password_encryption_type = tvb_get_ephemeral_unicode_string(tvb, offset, password_encryption_type_length*2, ENC_LITTLE_ENDIAN);

        proto_tree_add_string(tree, hf_msmms_command_password_type, tvb,
                              offset, password_encryption_type_length*2,
                              password_encryption_type);
    }
/
}
