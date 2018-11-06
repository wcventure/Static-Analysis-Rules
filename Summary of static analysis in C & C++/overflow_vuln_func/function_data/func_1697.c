static void dissect_client_player_info(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
                                       guint offset, guint length_remaining)
{
    char *player_info;

    /
    proto_tree_add_item(tree, hf_msmms_command_prefix1, tvb, offset, 4, ENC_LITTLE_ENDIAN);
    offset += 4;
    proto_tree_add_item(tree, hf_msmms_command_prefix2, tvb, offset, 4, ENC_LITTLE_ENDIAN);
    offset += 4;

    /
    offset += 4;

    /
    player_info = tvb_get_ephemeral_unicode_string(tvb, offset, length_remaining - 12, ENC_LITTLE_ENDIAN);

    proto_tree_add_string(tree, hf_msmms_command_client_player_info, tvb,
                          offset, length_remaining-12,
                          player_info);

    col_append_fstr(pinfo->cinfo, COL_INFO, " (%s)",
                    format_text((guchar*)player_info, (length_remaining - 12)/2));
}
