QEMUPutMouseEntry *qemu_add_mouse_event_handler(QEMUPutMouseEvent *func,
                                                void *opaque, int absolute,
                                                const char *name)
{
    QEMUPutMouseEntry *s;

    s = g_malloc0(sizeof(QEMUPutMouseEntry));

    s->qemu_put_mouse_event = func;
    s->qemu_put_mouse_event_opaque = opaque;
    s->qemu_put_mouse_event_absolute = absolute;

    s->h.name = name;
    s->h.mask = INPUT_EVENT_MASK_BTN |
        (absolute ? INPUT_EVENT_MASK_ABS : INPUT_EVENT_MASK_REL);
    s->h.event = legacy_mouse_event;
    s->h.sync = legacy_mouse_sync;
    s->s = qemu_input_handler_register((DeviceState *)s,
                                       &s->h);

    return s;
}
