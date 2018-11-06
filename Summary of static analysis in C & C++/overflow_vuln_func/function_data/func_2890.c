static int cirrus_vga_load(QEMUFile *f, void *opaque, int version_id)
{
    CirrusVGAState *s = opaque;
    int ret;

    if (version_id > 2)
        return -EINVAL;

    if (s->pci_dev && version_id >= 2) {
        ret = pci_device_load(s->pci_dev, f);
        if (ret < 0)
            return ret;
    }

    qemu_get_be32s(f, &s->latch);
    qemu_get_8s(f, &s->sr_index);
    qemu_get_buffer(f, s->sr, 256);
    qemu_get_8s(f, &s->gr_index);
    qemu_get_8s(f, &s->cirrus_shadow_gr0);
    qemu_get_8s(f, &s->cirrus_shadow_gr1);
    s->gr[0x00] = s->cirrus_shadow_gr0 & 0x0f;
    s->gr[0x01] = s->cirrus_shadow_gr1 & 0x0f;
    qemu_get_buffer(f, s->gr + 2, 254);
    qemu_get_8s(f, &s->ar_index);
    qemu_get_buffer(f, s->ar, 21);
    s->ar_flip_flop=qemu_get_be32(f);
    qemu_get_8s(f, &s->cr_index);
    qemu_get_buffer(f, s->cr, 256);
    qemu_get_8s(f, &s->msr);
    qemu_get_8s(f, &s->fcr);
    qemu_get_8s(f, &s->st00);
    qemu_get_8s(f, &s->st01);

    qemu_get_8s(f, &s->dac_state);
    qemu_get_8s(f, &s->dac_sub_index);
    qemu_get_8s(f, &s->dac_read_index);
    qemu_get_8s(f, &s->dac_write_index);
    qemu_get_buffer(f, s->dac_cache, 3);
    qemu_get_buffer(f, s->palette, 768);

    s->bank_offset=qemu_get_be32(f);

    qemu_get_8s(f, &s->cirrus_hidden_dac_lockindex);
    qemu_get_8s(f, &s->cirrus_hidden_dac_data);

    qemu_get_be32s(f, &s->hw_cursor_x);
    qemu_get_be32s(f, &s->hw_cursor_y);

    cirrus_update_memory_access(s);
    /
    s->graphic_mode = -1;
    cirrus_update_bank_ptr(s, 0);
    cirrus_update_bank_ptr(s, 1);
    return 0;
}
