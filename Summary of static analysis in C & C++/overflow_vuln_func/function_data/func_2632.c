void fw_cfg_add_bytes(FWCfgState *s, uint16_t key, uint8_t *data, uint32_t len)
{
    int arch = !!(key & FW_CFG_ARCH_LOCAL);

    key &= FW_CFG_ENTRY_MASK;

    assert(key < FW_CFG_MAX_ENTRY);

    s->entries[arch][key].data = data;
    s->entries[arch][key].len = len;
}
