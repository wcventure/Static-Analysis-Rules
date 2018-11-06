static void emulated_push_type(EmulatedState *card, uint32_t type)
{
    EmulEvent *event = (EmulEvent *)g_malloc(sizeof(EmulEvent));

    assert(event);
    event->p.gen.type = type;
    emulated_push_event(card, event);
}
