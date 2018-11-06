void *
codec_g726_init(void)
{
    memset (&state, 0, sizeof (state));
    g726_init(&state, 32000, 0, 1);

    return NULL;
}
