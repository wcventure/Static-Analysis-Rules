static void qmp_command_info(QmpCommand *cmd, void *opaque)
{
    GuestAgentInfo *info = opaque;
    GuestAgentCommandInfo *cmd_info;
    GuestAgentCommandInfoList *cmd_info_list;

    cmd_info = g_malloc0(sizeof(GuestAgentCommandInfo));
    cmd_info->name = g_strdup(qmp_command_name(cmd));
    cmd_info->enabled = qmp_command_is_enabled(cmd);
    cmd_info->success_response = qmp_has_success_response(cmd);

    cmd_info_list = g_malloc0(sizeof(GuestAgentCommandInfoList));
    cmd_info_list->value = cmd_info;
    cmd_info_list->next = info->supported_commands;
    info->supported_commands = cmd_info_list;
}
