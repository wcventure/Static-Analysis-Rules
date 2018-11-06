static int64_t get_pts(const char **buf, int *duration,
                       int32_t *x1, int32_t *y1, int32_t *x2, int32_t *y2)
{
    int i;

    for (i=0; i<2; i++) {
        int hh1, mm1, ss1, ms1;
        int hh2, mm2, ss2, ms2;
        if (sscanf(*buf, "%d:%2d:%2d%*1[,.]%3d --> %d:%2d:%2d%*1[,.]%3d"
                   "%*[ ]X1:%u X2:%u Y1:%u Y2:%u",
                   &hh1, &mm1, &ss1, &ms1,
                   &hh2, &mm2, &ss2, &ms2,
                   x1, x2, y1, y2) >= 8) {
            int64_t start = (hh1*3600LL + mm1*60LL + ss1) * 1000LL + ms1;
            int64_t end   = (hh2*3600LL + mm2*60LL + ss2) * 1000LL + ms2;
            *duration = end - start;
            *buf += strcspn(*buf, "\n") + 1;
            return start;
        }
        *buf += strcspn(*buf, "\n") + 1;
    }
    return AV_NOPTS_VALUE;
}
