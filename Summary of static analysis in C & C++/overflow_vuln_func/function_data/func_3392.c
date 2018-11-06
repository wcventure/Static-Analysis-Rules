static int64_t get_pts(const char *buf, int *duration)
{
    int i, hour, min, sec, hsec;
    int he, me, se, mse;

    for (i=0; i<2; i++) {
        int64_t start, end;
        if (sscanf(buf, "%d:%2d:%2d%*1[,.]%3d --> %d:%2d:%2d%*1[,.]%3d",
                   &hour, &min, &sec, &hsec, &he, &me, &se, &mse) == 8) {
            min += 60*hour;
            sec += 60*min;
            start = sec*1000+hsec;
            me += 60*he;
            se += 60*me;
            end = se*1000+mse;
            *duration = end - start;
            return start;
        }
        buf += strcspn(buf, "\n") + 1;
    }
    return AV_NOPTS_VALUE;
}
