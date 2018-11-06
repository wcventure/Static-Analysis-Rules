static void twos_complement (gint64 *v, guint8 bit_len)
{
    guint64 i, to_stuff;
    i = (guint64) (1 << (bit_len - 1));
    to_stuff = *v & i;
    i = 0;
    if (to_stuff) i = G_GINT64_CONSTANT(0xffffffffffffffff) << bit_len;
    *v = *v | i;
}
