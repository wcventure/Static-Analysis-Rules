struct GuestAgentInfo *qmp_guest_info(Error **errp)
{
    GuestAgentInfo *info = g_malloc0(sizeof(GuestAgentInfo));

    info->version = g_strdup(QEMU_VERSION);
    qmp_for_each_command(qmp_command_info, info);
    return info;
}
