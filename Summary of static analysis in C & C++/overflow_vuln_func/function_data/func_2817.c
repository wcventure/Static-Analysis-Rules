static void emulated_push_error(EmulatedState *card, uint64_t code)
{
    EmulEvent *event = (EmulEvent *)g_malloc(sizeof(EmulEvent));

    assert(event);
    event->p.error.type = EMUL_ERROR;
    event->p.error.code = code;
    emulated_push_event(card, event);
}
