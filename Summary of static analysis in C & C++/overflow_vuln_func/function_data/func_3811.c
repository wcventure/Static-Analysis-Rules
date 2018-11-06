    static inline void coeff_unpack_arith_##n(DiracArith *c, int qfactor, int qoffset, \
                                              SubBand *b, type *buf, int x, int y) \
    { \
        int coeff, sign, sign_pred = 0, pred_ctx = CTX_ZPZN_F1; \
        const int mstride = -(b->stride >> (1+b->pshift)); \
        if (b->parent) { \
            const type *pbuf = (type *)b->parent->ibuf; \
            const int stride = b->parent->stride >> (1+b->parent->pshift); \
            pred_ctx += !!pbuf[stride * (y>>1) + (x>>1)] << 1; \
        } \
        if (b->orientation == subband_hl) \
            sign_pred = buf[mstride]; \
        if (x) { \
            pred_ctx += !(buf[-1] | buf[mstride] | buf[-1 + mstride]); \
            if (b->orientation == subband_lh) \
                sign_pred = buf[-1]; \
        } else { \
            pred_ctx += !buf[mstride]; \
        } \
        coeff = dirac_get_arith_uint(c, pred_ctx, CTX_COEFF_DATA); \
        if (coeff) { \
            coeff = (coeff * qfactor + qoffset) >> 2; \
            sign  = dirac_get_arith_bit(c, SIGN_CTX(sign_pred)); \
            coeff = (coeff ^ -sign) + sign; \
        } \
        *buf = coeff; \
    } \
