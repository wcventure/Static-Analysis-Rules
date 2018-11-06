static void RENAME(vertical_compose53iL0)(uint8_t *_b0, uint8_t *_b1, uint8_t *_b2,
                                          int width)
{
    int i;
    TYPE *b0 = (TYPE *)_b0;
    TYPE *b1 = (TYPE *)_b1;
    TYPE *b2 = (TYPE *)_b2;
    for (i = 0; i < width; i++)
        b1[i] -= (b0[i] + b2[i] + 2) >> 2;
}
