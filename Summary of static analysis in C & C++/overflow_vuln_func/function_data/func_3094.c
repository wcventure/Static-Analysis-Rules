static int mm_decode_pal(MmContext *s)
{
    int i;

    bytestream2_skip(&s->gb, 4);
    for (i = 0; i < 128; i++) {
        s->palette[i] = 0xFF << 24 | bytestream2_get_be24(&s->gb);
        s->palette[i+128] = s->palette[i]<<2;
    }

    return 0;
}
