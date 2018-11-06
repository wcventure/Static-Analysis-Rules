void fw_cfg_add_callback(FWCfgState *s, uint16_t key, FWCfgCallback callback,
                         void *callback_opaque, uint8_t *data, size_t len)
{
    int arch = !!(key & FW_CFG_ARCH_LOCAL);

    assert(key & FW_CFG_WRITE_CHANNEL);

    key &= FW_CFG_ENTRY_MASK;

    assert(key < FW_CFG_MAX_ENTRY && len <= 65535);

    s->entries[arch][key].data = data;
    s->entries[arch][key].len = len;
    s->entries[arch][key].callback_opaque = callback_opaque;
    s->entries[arch][key].callback = callback;
}
