static void compute_scale_factors(unsigned char scale_code[SBLIMIT],
                                  unsigned char scale_factors[SBLIMIT][3], 
                                  int sb_samples[3][12][SBLIMIT],
                                  int sblimit)
{
    int *p, vmax, v, n, i, j, k, code;
    int index, d1, d2;
    unsigned char *sf = &scale_factors[0][0];
    
    for(j=0;j<sblimit;j++) {
        for(i=0;i<3;i++) {
            /
            p = &sb_samples[i][0][j];
            vmax = abs(*p);
            for(k=1;k<12;k++) {
                p += SBLIMIT;
                v = abs(*p);
                if (v > vmax)
                    vmax = v;
            }
            /
            if (vmax > 0) {
                n = av_log2(vmax);
                /
                index = (21 - n) * 3 - 3;
                if (index >= 0) {
                    while (vmax <= scale_factor_table[index+1])
                        index++;
                } else {
                    index = 0; /
                }
            } else {
                index = 63;
            }
            
#if 0
            printf("%2d:%d in=%x %x %d\n", 
                   j, i, vmax, scale_factor_table[index], index);
#endif
            /
            assert(index >=0 && index <= 63);
            sf[i] = index;
        }

        /
        d1 = scale_diff_table[sf[0] - sf[1] + 64];
        d2 = scale_diff_table[sf[1] - sf[2] + 64];
        
        /
        switch(d1 * 5 + d2) {
        case 0*5+0:
        case 0*5+4:
        case 3*5+4:
        case 4*5+0:
        case 4*5+4:
            code = 0;
            break;
        case 0*5+1:
        case 0*5+2:
        case 4*5+1:
        case 4*5+2:
            code = 3;
            sf[2] = sf[1];
            break;
        case 0*5+3:
        case 4*5+3:
            code = 3;
            sf[1] = sf[2];
            break;
        case 1*5+0:
        case 1*5+4:
        case 2*5+4:
            code = 1;
            sf[1] = sf[0];
            break;
        case 1*5+1:
        case 1*5+2:
        case 2*5+0:
        case 2*5+1:
        case 2*5+2:
            code = 2;
            sf[1] = sf[2] = sf[0];
            break;
        case 2*5+3:
        case 3*5+3:
            code = 2;
            sf[0] = sf[1] = sf[2];
            break;
        case 3*5+0:
        case 3*5+1:
        case 3*5+2:
            code = 2;
            sf[0] = sf[2] = sf[1];
            break;
        case 1*5+3:
            code = 2;
            if (sf[0] > sf[2])
              sf[0] = sf[2];
            sf[1] = sf[2] = sf[0];
            break;
        default:
            abort();
        }
        
#if 0
        printf("%d: %2d %2d %2d %d %d -> %d\n", j, 
               sf[0], sf[1], sf[2], d1, d2, code);
#endif
        scale_code[j] = code;
        sf += 3;
    }
}
