static OfDpaGroup *of_dpa_group_alloc(uint32_t id)
{
    OfDpaGroup *group = g_malloc0(sizeof(OfDpaGroup));

    if (!group) {
        return NULL;
    }

    group->id = id;

    return group;
}
