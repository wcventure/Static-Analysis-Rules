static inline
void compute_images_mse(PSNRContext *s,
                        const uint8_t *main_data[4], const int main_linesizes[4],
                        const uint8_t *ref_data[4], const int ref_linesizes[4],
                        int w, int h, double mse[4])
{
    int i, c, j;

    for (c = 0; c < s->nb_components; c++) {
        const int outw = s->planewidth[c];
        const int outh = s->planeheight[c];
        const uint8_t *main_line = main_data[c];
        const uint8_t *ref_line = ref_data[c];
        const int ref_linesize = ref_linesizes[c];
        const int main_linesize = main_linesizes[c];
        uint64_t m = 0;

        for (i = 0; i < outh; i++) {
            int m2 = 0;
            for (j = 0; j < outw; j++)
                m2 += pow2(main_line[j] - ref_line[j]);
            m += m2;
            ref_line += ref_linesize;
            main_line += main_linesize;
        }
        mse[c] = m / (double)(outw * outh);
    }
}
