static void quantize_mantissas_blk_ch(AC3EncodeContext *s, int32_t *fixed_coef,
                                      int8_t exp_shift, uint8_t *exp,
                                      uint8_t *bap, uint16_t *qmant, int n)
{
    int i;

    for (i = 0; i < n; i++) {
        int v;
        int c = fixed_coef[i];
        int e = exp[i] - exp_shift;
        int b = bap[i];
        switch (b) {
        case 0:
            v = 0;
            break;
        case 1:
            v = sym_quant(c, e, 3);
            switch (s->mant1_cnt) {
            case 0:
                s->qmant1_ptr = &qmant[i];
                v = 9 * v;
                s->mant1_cnt = 1;
                break;
            case 1:
                *s->qmant1_ptr += 3 * v;
                s->mant1_cnt = 2;
                v = 128;
                break;
            default:
                *s->qmant1_ptr += v;
                s->mant1_cnt = 0;
                v = 128;
                break;
            }
            break;
        case 2:
            v = sym_quant(c, e, 5);
            switch (s->mant2_cnt) {
            case 0:
                s->qmant2_ptr = &qmant[i];
                v = 25 * v;
                s->mant2_cnt = 1;
                break;
            case 1:
                *s->qmant2_ptr += 5 * v;
                s->mant2_cnt = 2;
                v = 128;
                break;
            default:
                *s->qmant2_ptr += v;
                s->mant2_cnt = 0;
                v = 128;
                break;
            }
            break;
        case 3:
            v = sym_quant(c, e, 7);
            break;
        case 4:
            v = sym_quant(c, e, 11);
            switch (s->mant4_cnt) {
            case 0:
                s->qmant4_ptr = &qmant[i];
                v = 11 * v;
                s->mant4_cnt = 1;
                break;
            default:
                *s->qmant4_ptr += v;
                s->mant4_cnt = 0;
                v = 128;
                break;
            }
            break;
        case 5:
            v = sym_quant(c, e, 15);
            break;
        case 14:
            v = asym_quant(c, e, 14);
            break;
        case 15:
            v = asym_quant(c, e, 16);
            break;
        default:
            v = asym_quant(c, e, b - 1);
            break;
        }
        qmant[i] = v;
    }
}
