static int normalize_bits(int num, int width)
{
    if (!num)
        return 0;
    if (num == -1)
        return width;
    if (num < 0)
        num = ~num;

    return width - av_log2(num);
}
