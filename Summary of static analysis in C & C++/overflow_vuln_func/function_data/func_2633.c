void fw_cfg_add_string(FWCfgState *s, uint16_t key, const char *value)
{
    size_t sz = strlen(value) + 1;

    return fw_cfg_add_bytes(s, key, (uint8_t *)g_memdup(value, sz), sz);
}
