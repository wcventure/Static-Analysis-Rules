void *
codec_g722_init(void)
{
    memset (&state, 0, sizeof (state));
    g722_decode_init(&state, 64000, 0);

    return NULL;
}
