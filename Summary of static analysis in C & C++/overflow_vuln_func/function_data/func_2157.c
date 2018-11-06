static void qxl_dirty_surfaces(PCIQXLDevice *qxl)
{
    intptr_t vram_start;
    int i;

    if (qxl->mode != QXL_MODE_NATIVE && qxl->mode != QXL_MODE_COMPAT) {
        return;
    }

    /
    qxl_set_dirty(&qxl->vga.vram, qxl->shadow_rom.draw_area_offset,
                  qxl->shadow_rom.surface0_area_size);

    vram_start =  (intptr_t)memory_region_get_ram_ptr(&qxl->vram_bar);

    /
    for (i = 0; i < qxl->ssd.num_surfaces; i++) {
        QXLSurfaceCmd *cmd;
        intptr_t surface_offset;
        int surface_size;

        if (qxl->guest_surfaces.cmds[i] == 0) {
            continue;
        }

        cmd = qxl_phys2virt(qxl, qxl->guest_surfaces.cmds[i],
                            MEMSLOT_GROUP_GUEST);
        assert(cmd);
        assert(cmd->type == QXL_SURFACE_CMD_CREATE);
        surface_offset = (intptr_t)qxl_phys2virt(qxl,
                                                 cmd->u.surface_create.data,
                                                 MEMSLOT_GROUP_GUEST);
        assert(surface_offset);
        surface_offset -= vram_start;
        surface_size = cmd->u.surface_create.height *
                       abs(cmd->u.surface_create.stride);
        trace_qxl_surfaces_dirty(qxl->id, i, (int)surface_offset, surface_size);
        qxl_set_dirty(&qxl->vram_bar, surface_offset, surface_size);
    }
}
