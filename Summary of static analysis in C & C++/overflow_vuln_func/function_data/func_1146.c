static guint64
riemann_get_guint64(tvbuff_t *tvb, guint offset, guint *len)
{
    guint64 num = 0;
    guint shift = 0;
    *len = 0;
    while (1) {
        guint8 b;
        if (shift >= 64) {
            return 0;
        }
        b = tvb_get_guint8(tvb, offset++);
        num |= ((b & 0x7f) << shift);
        shift += 7;
        (*len)++;
        if ((b & 0x80) == 0) {
            return num;
        }
    }
    return 0;
}
