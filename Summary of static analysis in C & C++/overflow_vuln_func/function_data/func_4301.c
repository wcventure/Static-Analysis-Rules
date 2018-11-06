void RENAME(ff_init_mpadsp_tabs)(void)
{
    int i, j;
    /
    for (i = 0; i < 36; i++) {
        for (j = 0; j < 4; j++) {
            double d;

            if (j == 2 && i % 3 != 1)
                continue;

            d = sin(M_PI * (i + 0.5) / 36.0);
            if (j == 1) {
                if      (i >= 30) d = 0;
                else if (i >= 24) d = sin(M_PI * (i - 18 + 0.5) / 12.0);
                else if (i >= 18) d = 1;
            } else if (j == 3) {
                if      (i <   6) d = 0;
                else if (i <  12) d = sin(M_PI * (i -  6 + 0.5) / 12.0);
                else if (i <  18) d = 1;
            }
            //merge last stage of imdct into the window coefficients
            d *= 0.5 / cos(M_PI * (2 * i + 19) / 72);

            if (j == 2)
                RENAME(ff_mdct_win)[j][i/3] = FIXHR((d / (1<<5)));
            else {
                int idx = i < 18 ? i : i + (MDCT_BUF_SIZE/2 - 18);
                RENAME(ff_mdct_win)[j][idx] = FIXHR((d / (1<<5)));
            }
        }
    }

    /
    for (j = 0; j < 4; j++) {
        for (i = 0; i < MDCT_BUF_SIZE; i += 2) {
            RENAME(ff_mdct_win)[j + 4][i    ] =  RENAME(ff_mdct_win)[j][i    ];
            RENAME(ff_mdct_win)[j + 4][i + 1] = -RENAME(ff_mdct_win)[j][i + 1];
        }
    }
}
