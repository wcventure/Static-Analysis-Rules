static inline void tm2_high_chroma(int *data, int stride, int *last, int *CD, int *deltas)
{
    int i, j;
    for (j = 0; j < 2; j++) {
        for (i = 0; i < 2; i++)  {
            CD[j]   += deltas[i + j * 2];
            last[i] += CD[j];
            data[i]  = last[i];
        }
        data += stride;
    }
}
