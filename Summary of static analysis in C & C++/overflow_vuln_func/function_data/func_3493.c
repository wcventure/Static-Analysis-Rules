static int get_std_framerate(int i)
{
    if (i < 60 * 12)
        return i * 1001;
    else
        return ((const int[]) { 24, 30, 60, 12, 15 })[i - 60 * 12] * 1000 * 12;
}
