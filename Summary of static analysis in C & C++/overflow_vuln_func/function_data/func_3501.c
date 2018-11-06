static void count_usage(uint8_t *src, int width,
                        int height, uint32_t *counts)
{
    int i, j;

    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            counts[src[i]]++;
        }
        src += width;
    }
}
