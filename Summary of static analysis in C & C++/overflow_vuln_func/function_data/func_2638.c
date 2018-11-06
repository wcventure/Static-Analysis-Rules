void fw_cfg_add_file(FWCfgState *s,  const char *filename, uint8_t *data,
                     uint32_t len)
{
    int i, index;

    if (!s->files) {
        int dsize = sizeof(uint32_t) + sizeof(FWCfgFile) * FW_CFG_FILE_SLOTS;
        s->files = g_malloc0(dsize);
        fw_cfg_add_bytes(s, FW_CFG_FILE_DIR, (uint8_t*)s->files, dsize);
    }

    index = be32_to_cpu(s->files->count);
    assert(index < FW_CFG_FILE_SLOTS);

    fw_cfg_add_bytes(s, FW_CFG_FILE_FIRST + index, data, len);

    pstrcpy(s->files->f[index].name, sizeof(s->files->f[index].name),
            filename);
    for (i = 0; i < index; i++) {
        if (strcmp(s->files->f[index].name, s->files->f[i].name) == 0) {
            trace_fw_cfg_add_file_dupe(s, s->files->f[index].name);
            return;
        }
    }

    s->files->f[index].size   = cpu_to_be32(len);
    s->files->f[index].select = cpu_to_be16(FW_CFG_FILE_FIRST + index);
    trace_fw_cfg_add_file(s, index, s->files->f[index].name, len);

    s->files->count = cpu_to_be32(index+1);
}
