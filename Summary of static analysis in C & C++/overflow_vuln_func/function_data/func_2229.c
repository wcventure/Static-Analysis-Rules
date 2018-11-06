static void qxl_create_guest_primary(PCIQXLDevice *qxl, int loadvm,
                                     qxl_async_io async)
{
    QXLDevSurfaceCreate surface;
    QXLSurfaceCreate *sc = &qxl->guest_primary.surface;
    int size;
    int requested_height = le32_to_cpu(sc->height);
    int requested_stride = le32_to_cpu(sc->stride);

    size = abs(requested_stride) * requested_height;
    if (size > qxl->vgamem_size) {
        qxl_set_guest_bug(qxl, "%s: requested primary larger then framebuffer"
                               " size", __func__);
        return;
    }

    if (qxl->mode == QXL_MODE_NATIVE) {
        qxl_set_guest_bug(qxl, "%s: nop since already in QXL_MODE_NATIVE",
                      __func__);
    }
    qxl_exit_vga_mode(qxl);

    surface.format     = le32_to_cpu(sc->format);
    surface.height     = le32_to_cpu(sc->height);
    surface.mem        = le64_to_cpu(sc->mem);
    surface.position   = le32_to_cpu(sc->position);
    surface.stride     = le32_to_cpu(sc->stride);
    surface.width      = le32_to_cpu(sc->width);
    surface.type       = le32_to_cpu(sc->type);
    surface.flags      = le32_to_cpu(sc->flags);
    trace_qxl_create_guest_primary(qxl->id, sc->width, sc->height, sc->mem,
                                   sc->format, sc->position);
    trace_qxl_create_guest_primary_rest(qxl->id, sc->stride, sc->type,
                                        sc->flags);

    if ((surface.stride & 0x3) != 0) {
        qxl_set_guest_bug(qxl, "primary surface stride = %d %% 4 != 0",
                          surface.stride);
        return;
    }

    surface.mouse_mode = true;
    surface.group_id   = MEMSLOT_GROUP_GUEST;
    if (loadvm) {
        surface.flags |= QXL_SURF_FLAG_KEEP_DATA;
    }

    qxl->mode = QXL_MODE_NATIVE;
    qxl->cmdflags = 0;
    qemu_spice_create_primary_surface(&qxl->ssd, 0, &surface, async);

    if (async == QXL_SYNC) {
        qxl_create_guest_primary_complete(qxl);
    }
}
