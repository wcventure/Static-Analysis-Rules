static void map_val_34_to_20(INTFLOAT par[PS_MAX_NR_IIDICC])
{
#if USE_FIXED
    par[ 0] = (int)(((int64_t)(par[ 0] + (par[ 1]>>1)) * 1431655765 + \
                      0x40000000) >> 31);
    par[ 1] = (int)(((int64_t)((par[ 1]>>1) + par[ 2]) * 1431655765 + \
                      0x40000000) >> 31);
    par[ 2] = (int)(((int64_t)(par[ 3] + (par[ 4]>>1)) * 1431655765 + \
                      0x40000000) >> 31);
    par[ 3] = (int)(((int64_t)((par[ 4]>>1) + par[ 5]) * 1431655765 + \
                      0x40000000) >> 31);
#else
    par[ 0] = (2*par[ 0] +   par[ 1]) * 0.33333333f;
    par[ 1] = (  par[ 1] + 2*par[ 2]) * 0.33333333f;
    par[ 2] = (2*par[ 3] +   par[ 4]) * 0.33333333f;
    par[ 3] = (  par[ 4] + 2*par[ 5]) * 0.33333333f;
#endif /
    par[ 4] = AAC_HALF_SUM(par[ 6], par[ 7]);
    par[ 5] = AAC_HALF_SUM(par[ 8], par[ 9]);
    par[ 6] =    par[10];
    par[ 7] =    par[11];
    par[ 8] = AAC_HALF_SUM(par[12], par[13]);
    par[ 9] = AAC_HALF_SUM(par[14], par[15]);
    par[10] =    par[16];
    par[11] =    par[17];
    par[12] =    par[18];
    par[13] =    par[19];
    par[14] = AAC_HALF_SUM(par[20], par[21]);
    par[15] = AAC_HALF_SUM(par[22], par[23]);
    par[16] = AAC_HALF_SUM(par[24], par[25]);
    par[17] = AAC_HALF_SUM(par[26], par[27]);
#if USE_FIXED
    par[18] = (((par[28]+2)>>2) + ((par[29]+2)>>2) + ((par[30]+2)>>2) + ((par[31]+2)>>2));
#else
    par[18] = (  par[28] +   par[29] +   par[30] +   par[31]) * 0.25f;
#endif /
    par[19] = AAC_HALF_SUM(par[32], par[33]);
}
