static int dot_product(const int16_t *a, const int16_t *b, int length)
{
    int i, sum = 0;

    for (i = 0; i < length; i++) {
        int64_t prod = av_clipl_int32(MUL64(a[i], b[i]) << 1);
        sum = av_clipl_int32(sum + prod);
    }
    return sum;
}
