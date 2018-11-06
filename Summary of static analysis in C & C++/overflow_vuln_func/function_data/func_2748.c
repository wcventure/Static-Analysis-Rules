QEMUPutLEDEntry *qemu_add_led_event_handler(QEMUPutLEDEvent *func,
                                            void *opaque)
{
    QEMUPutLEDEntry *s;

    s = g_malloc0(sizeof(QEMUPutLEDEntry));

    s->put_led = func;
    s->opaque = opaque;
    QTAILQ_INSERT_TAIL(&led_handlers, s, next);
    return s;
}
