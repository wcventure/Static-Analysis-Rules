int register_savevm(DeviceState *dev,
                    const char *idstr,
                    int instance_id,
                    int version_id,
                    SaveStateHandler *save_state,
                    LoadStateHandler *load_state,
                    void *opaque)
{
    SaveVMHandlers *ops = g_malloc0(sizeof(SaveVMHandlers));
    ops->save_state = save_state;
    ops->load_state = load_state;
    return register_savevm_live(dev, idstr, instance_id, version_id,
                                ops, opaque);
}
