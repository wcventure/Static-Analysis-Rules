static MTPData *usb_mtp_get_object_info(MTPState *s, MTPControl *c,
                                        MTPObject *o)
{
    MTPData *d = usb_mtp_data_alloc(c);

    trace_usb_mtp_op_get_object_info(s->dev.addr, o->handle, o->path);

    usb_mtp_add_u32(d, QEMU_STORAGE_ID);
    usb_mtp_add_u16(d, o->format);
    usb_mtp_add_u16(d, 0);
    usb_mtp_add_u32(d, o->stat.st_size);

    usb_mtp_add_u16(d, 0);
    usb_mtp_add_u32(d, 0);
    usb_mtp_add_u32(d, 0);
    usb_mtp_add_u32(d, 0);
    usb_mtp_add_u32(d, 0);
    usb_mtp_add_u32(d, 0);
    usb_mtp_add_u32(d, 0);

    if (o->parent) {
        usb_mtp_add_u32(d, o->parent->handle);
    } else {
        usb_mtp_add_u32(d, 0);
    }
    if (o->format == FMT_ASSOCIATION) {
        usb_mtp_add_u16(d, 0x0001);
        usb_mtp_add_u32(d, 0x00000001);
        usb_mtp_add_u32(d, 0);
    } else {
        usb_mtp_add_u16(d, 0);
        usb_mtp_add_u32(d, 0);
        usb_mtp_add_u32(d, 0);
    }

    usb_mtp_add_str(d, o->name);
    usb_mtp_add_time(d, o->stat.st_ctime);
    usb_mtp_add_time(d, o->stat.st_mtime);
    usb_mtp_add_wstr(d, L"");

    return d;
}
