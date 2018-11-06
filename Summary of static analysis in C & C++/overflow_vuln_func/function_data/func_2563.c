int vmstate_register_with_alias_id(DeviceState *dev, int instance_id,
                                   const VMStateDescription *vmsd,
                                   void *opaque, int alias_id,
                                   int required_for_version)
{
    SaveStateEntry *se;

    /
    assert(alias_id == -1 || required_for_version >= vmsd->minimum_version_id);

    se = g_malloc0(sizeof(SaveStateEntry));
    se->version_id = vmsd->version_id;
    se->section_id = savevm_state.global_section_id++;
    se->opaque = opaque;
    se->vmsd = vmsd;
    se->alias_id = alias_id;

    if (dev) {
        char *id = qdev_get_dev_path(dev);
        if (id) {
            pstrcpy(se->idstr, sizeof(se->idstr), id);
            pstrcat(se->idstr, sizeof(se->idstr), "/");
            g_free(id);

            se->compat = g_malloc0(sizeof(CompatEntry));
            pstrcpy(se->compat->idstr, sizeof(se->compat->idstr), vmsd->name);
            se->compat->instance_id = instance_id == -1 ?
                         calculate_compat_instance_id(vmsd->name) : instance_id;
            instance_id = -1;
        }
    }
    pstrcat(se->idstr, sizeof(se->idstr), vmsd->name);

    if (instance_id == -1) {
        se->instance_id = calculate_new_instance_id(se->idstr);
    } else {
        se->instance_id = instance_id;
    }
    assert(!se->compat || se->instance_id == 0);
    /
    QTAILQ_INSERT_TAIL(&savevm_state.handlers, se, entry);
    return 0;
}
