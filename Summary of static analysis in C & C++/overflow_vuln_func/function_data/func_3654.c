            // copy back and deinterleave
            for (i =   mh; i < lh; i+=2, j++)
                t[w*lp + j] = l[i];
            for (i = 1-mh; i < lh; i+=2, j++)
                t[w*lp + j] = l[i];
        }
    }
}
static void sd_1d97_float(float *p, int i0, int i1)
{
    int i;

    if (i1 <= i0 + 1) {
        if (i0 == 1)
            p[1] *= F_LFTG_X;
        else
            p[0] *= F_LFTG_K;
        return;
    }

    extend97_float(p, i0, i1);
    i0++; i1++;

    for (i = i0/2 - 2; i < i1/2 + 1; i++)
        p[2*i+1] -= 1.586134 * (p[2*i] + p[2*i+2]);
    for (i = i0/2 - 1; i < i1/2 + 1; i++)
        p[2*i] -= 0.052980 * (p[2*i-1] + p[2*i+1]);
    for (i = i0/2 - 1; i < i1/2; i++)
        p[2*i+1] += 0.882911 * (p[2*i] + p[2*i+2]);
    for (i = i0/2; i < i1/2; i++)
        p[2*i] += 0.443506 * (p[2*i-1] + p[2*i+1]);
}
