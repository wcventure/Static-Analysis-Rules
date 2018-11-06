static av_always_inline void autocorrelate(const int x[40][2], SoftFloat phi[3][2][2], int lag)
{
    int i;
    int64_t real_sum, imag_sum;
    int64_t accu_re = 0, accu_im = 0;

    if (lag) {
        for (i = 1; i < 38; i++) {
            accu_re += (int64_t)x[i][0] * x[i+lag][0];
            accu_re += (int64_t)x[i][1] * x[i+lag][1];
            accu_im += (int64_t)x[i][0] * x[i+lag][1];
            accu_im -= (int64_t)x[i][1] * x[i+lag][0];
        }

        real_sum = accu_re;
        imag_sum = accu_im;

        accu_re += (int64_t)x[ 0][0] * x[lag][0];
        accu_re += (int64_t)x[ 0][1] * x[lag][1];
        accu_im += (int64_t)x[ 0][0] * x[lag][1];
        accu_im -= (int64_t)x[ 0][1] * x[lag][0];

        phi[2-lag][1][0] = autocorr_calc(accu_re);
        phi[2-lag][1][1] = autocorr_calc(accu_im);

        if (lag == 1) {
            accu_re = real_sum;
            accu_im = imag_sum;
            accu_re += (int64_t)x[38][0] * x[39][0];
            accu_re += (int64_t)x[38][1] * x[39][1];
            accu_im += (int64_t)x[38][0] * x[39][1];
            accu_im -= (int64_t)x[38][1] * x[39][0];

            phi[0][0][0] = autocorr_calc(accu_re);
            phi[0][0][1] = autocorr_calc(accu_im);
        }
    } else {
        for (i = 1; i < 38; i++) {
            accu_re += (int64_t)x[i][0] * x[i][0];
            accu_re += (int64_t)x[i][1] * x[i][1];
        }
        real_sum = accu_re;
        accu_re += (int64_t)x[ 0][0] * x[ 0][0];
        accu_re += (int64_t)x[ 0][1] * x[ 0][1];

        phi[2][1][0] = autocorr_calc(accu_re);

        accu_re = real_sum;
        accu_re += (int64_t)x[38][0] * x[38][0];
        accu_re += (int64_t)x[38][1] * x[38][1];

        phi[1][0][0] = autocorr_calc(accu_re);
    }
}
