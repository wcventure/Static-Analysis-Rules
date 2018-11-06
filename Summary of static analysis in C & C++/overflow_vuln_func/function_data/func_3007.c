void ga_command_state_add(GACommandState *cs,
                          void (*init)(void),
                          void (*cleanup)(void))
{
    GACommandGroup *cg = g_malloc0(sizeof(GACommandGroup));
    cg->init = init;
    cg->cleanup = cleanup;
    cs->groups = g_slist_append(cs->groups, cg);
}
