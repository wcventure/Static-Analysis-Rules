#if defined(DEBUG)
void sample_dump(int fnum, INT32 *tab, int n)
{
    static FILE *files[16], *f;
    char buf[512];

    f = files[fnum];
    if (!f) {
        sprintf(buf, "/tmp/out%d.pcm", fnum);
        f = fopen(buf, "w");
        if (!f)
            return;
        files[fnum] = f;
    }
    
    if (fnum == 0) {
        int i;
        static int pos = 0;
        printf("pos=%d\n", pos);
        for(i=0;i<n;i++) {
            printf(" %f", (double)tab[i] / 32768.0);
            if ((i % 18) == 17)
                printf("\n");
        }
        pos += n;
    }

    fwrite(tab, 1, n * sizeof(INT32), f);
}
