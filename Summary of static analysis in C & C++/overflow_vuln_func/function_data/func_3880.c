static void final(const short *i1, const short *i2,
                  void *out, int *statbuf, int len)
{
    int x, i;
    unsigned short int work[50];
    short *ptr = work;

    memcpy(work, statbuf,20);
    memcpy(work + 10, i2, len * 2);

    for (i=0; i<len; i++) {
        int sum = 0;

        for(x=0; x<10; x++)
            sum += i1[9-x] * ptr[x];

        sum >>= 12;

        if (ptr[10] - sum < -32768 || ptr[10] - sum > 32767) {
            memset(out, 0, len * 2);
            memset(statbuf, 0, 20);
            return;
        }

        ptr[10] -= sum;
        ptr++;
    }

    memcpy(out, work+10, len * 2);
    memcpy(statbuf, work + 40, 20);
}
