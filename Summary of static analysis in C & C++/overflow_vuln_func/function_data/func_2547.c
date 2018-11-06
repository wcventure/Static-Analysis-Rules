static int qxl_post_load(void *opaque, int version)
{
    PCIQXLDevice* d = opaque;
    uint8_t *ram_start = d->vga.vram_ptr;
    QXLCommandExt *cmds;
    int in, out, newmode;

    assert(d->last_release_offset < d->vga.vram_size);
    if (d->last_release_offset == 0) {
        d->last_release = NULL;
    } else {
        d->last_release = (QXLReleaseInfo *)(ram_start + d->last_release_offset);
    }

    d->modes = (QXLModes*)((uint8_t*)d->rom + d->rom->modes_offset);

    trace_qxl_post_load(d->id, qxl_mode_to_string(d->mode));
    newmode = d->mode;
    d->mode = QXL_MODE_UNDEFINED;

    switch (newmode) {
    case QXL_MODE_UNDEFINED:
        qxl_create_memslots(d);
        break;
    case QXL_MODE_VGA:
        qxl_create_memslots(d);
        qxl_enter_vga_mode(d);
        break;
    case QXL_MODE_NATIVE:
        qxl_create_memslots(d);
        qxl_create_guest_primary(d, 1, QXL_SYNC);

        /
        cmds = g_malloc0(sizeof(QXLCommandExt) * (d->ssd.num_surfaces + 1));
        for (in = 0, out = 0; in < d->ssd.num_surfaces; in++) {
            if (d->guest_surfaces.cmds[in] == 0) {
                continue;
            }
            cmds[out].cmd.data = d->guest_surfaces.cmds[in];
            cmds[out].cmd.type = QXL_CMD_SURFACE;
            cmds[out].group_id = MEMSLOT_GROUP_GUEST;
            out++;
        }
        if (d->guest_cursor) {
            cmds[out].cmd.data = d->guest_cursor;
            cmds[out].cmd.type = QXL_CMD_CURSOR;
            cmds[out].group_id = MEMSLOT_GROUP_GUEST;
            out++;
        }
        qxl_spice_loadvm_commands(d, cmds, out);
        g_free(cmds);
        if (d->guest_monitors_config) {
            qxl_spice_monitors_config_async(d, 1);
        }
        break;
    case QXL_MODE_COMPAT:
        /
        qxl_set_mode(d, d->shadow_rom.mode, 1);
        break;
    }
    return 0;
}
