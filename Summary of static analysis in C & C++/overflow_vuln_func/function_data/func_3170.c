static void arith2_normalise(ArithCoder *c)
{
    while ((c->high >> 15) - (c->low >> 15) < 2) {
        if ((c->low ^ c->high) & 0x10000) {
            c->high  ^= 0x8000;
            c->value ^= 0x8000;
            c->low   ^= 0x8000;
        }
        c->high  = c->high  << 8 & 0xFFFFFF | 0xFF;
        c->value = c->value << 8 & 0xFFFFFF | bytestream2_get_byte(c->gbc.gB);
        c->low   = c->low   << 8 & 0xFFFFFF;
    }
}
