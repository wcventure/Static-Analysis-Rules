static void fix_bitshift(ShortenContext *s, int32_t *buffer)
{
    int i;

    if (s->bitshift != 0)
        for (i = 0; i < s->blocksize; i++)
            buffer[s->nwrap + i] <<= s->bitshift;
}
