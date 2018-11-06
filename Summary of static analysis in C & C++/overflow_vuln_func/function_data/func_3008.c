GACommandState *ga_command_state_new(void)
{
    GACommandState *cs = g_malloc0(sizeof(GACommandState));
    cs->groups = NULL;
    return cs;
}
