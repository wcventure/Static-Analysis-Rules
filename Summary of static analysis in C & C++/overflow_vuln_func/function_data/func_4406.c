int main(int argc, char *argv[])
{
    int i, j;
    uint64_t sse = 0;
    uint64_t dev;
    FILE *f[2];
    uint8_t buf[2][SIZE];
    uint64_t psnr;
    int len = 1;
    int64_t max;
    int shift      = argc < 5 ? 0 : atoi(argv[4]);
    int skip_bytes = argc < 6 ? 0 : atoi(argv[5]);
    int size0      = 0;
    int size1      = 0;
    int maxdist    = 0;

    if (argc < 3) {
        printf("tiny_psnr <file1> <file2> [<elem size> [<shift> [<skip bytes>]]]\n");
        printf("WAV headers are skipped automatically.\n");
        return 1;
    }

    if (argc > 3) {
        if (!strcmp(argv[3], "u8")) {
            len = 1;
        } else if (!strcmp(argv[3], "s16")) {
            len = 2;
        } else if (!strcmp(argv[3], "f32")) {
            len = 4;
        } else {
            char *end;
            len = strtol(argv[3], &end, 0);
            if (*end || len < 1 || len > 2) {
                fprintf(stderr, "Unsupported sample format: %s\n", argv[3]);
                return 1;
            }
        }
    }

    max = (1 << (8 * len)) - 1;

    f[0] = fopen(argv[1], "rb");
    f[1] = fopen(argv[2], "rb");
    if (!f[0] || !f[1]) {
        fprintf(stderr, "Could not open input files.\n");
        return 1;
    }

    for (i = 0; i < 2; i++) {
        uint8_t *p = buf[i];
        if (fread(p, 1, 12, f[i]) != 12)
            return 1;
        if (!memcmp(p, "RIFF", 4) &&
            !memcmp(p + 8, "WAVE", 4)) {
            if (fread(p, 1, 8, f[i]) != 8)
                return 1;
            while (memcmp(p, "data", 4)) {
                int s = p[4] | p[5] << 8 | p[6] << 16 | p[7] << 24;
                fseek(f[i], s, SEEK_CUR);
                if (fread(p, 1, 8, f[i]) != 8)
                    return 1;
            }
        } else {
            fseek(f[i], -12, SEEK_CUR);
        }
    }

    fseek(f[shift < 0], abs(shift), SEEK_CUR);

    fseek(f[0], skip_bytes, SEEK_CUR);
    fseek(f[1], skip_bytes, SEEK_CUR);

    for (;;) {
        int s0 = fread(buf[0], 1, SIZE, f[0]);
        int s1 = fread(buf[1], 1, SIZE, f[1]);

        for (j = 0; j < FFMIN(s0, s1); j += len) {
            int64_t a = buf[0][j];
            int64_t b = buf[1][j];
            int dist;
            if (len == 2) {
                a = get_s16l(buf[0] + j);
                b = get_s16l(buf[1] + j);
            } else if (len == 4) {
                a = get_f32l(buf[0] + j) * (1 << 24);
                b = get_f32l(buf[1] + j) * (1 << 24);
            } else {
                a = buf[0][j];
                b = buf[1][j];
            }
            sse += (a - b) * (a - b);
            dist = abs(a - b);
            if (dist > maxdist)
                maxdist = dist;
        }
        size0 += s0;
        size1 += s1;
        if (s0 + s1 <= 0)
            break;
    }

    i = FFMIN(size0, size1) / len;
    if (!i)
        i = 1;
    dev = int_sqrt(((sse / i) * F * F) + (((sse % i) * F * F) + i / 2) / i);
    if (sse)
        psnr = ((2 * log16(max << 16) + log16(i) - log16(sse)) *
                284619LL * F + (1LL << 31)) / (1LL << 32);
    else
        psnr = 1000 * F - 1; // floating point free infinity :)

    printf("stddev:%5d.%02d PSNR:%3d.%02d MAXDIFF:%5d bytes:%9d/%9d\n",
           (int)(dev / F), (int)(dev % F),
           (int)(psnr / F), (int)(psnr % F),
           maxdist, size0, size1);
    return 0;
}
