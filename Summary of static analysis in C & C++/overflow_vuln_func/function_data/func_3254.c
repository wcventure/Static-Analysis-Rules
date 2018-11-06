#if defined(DEBUG)
void sample_dump(int fnum, int32_t *tab, int n)
{
    static FILE *files[16], *f;
    char buf[512];
    int i;
    int32_t v;
    
    f = files[fnum];
    if (!f) {
        snprintf(buf, sizeof(buf), "/tmp/out%d.%s.pcm", 
                fnum, 
#ifdef USE_HIGHPRECISION
                "hp"
#else
                "lp"
#endif
                );
        f = fopen(buf, "w");
        if (!f)
            return;
        files[fnum] = f;
    }
    
    if (fnum == 0) {
        static int pos = 0;
        printf("pos=%d\n", pos);
        for(i=0;i<n;i++) {
            printf(" %0.4f", (double)tab[i] / FRAC_ONE);
            if ((i % 18) == 17)
                printf("\n");
        }
        pos += n;
    }
    for(i=0;i<n;i++) {
        /
        v = tab[i] << (23 - FRAC_BITS);
        fwrite(&v, 1, sizeof(int32_t), f);
    }
}
