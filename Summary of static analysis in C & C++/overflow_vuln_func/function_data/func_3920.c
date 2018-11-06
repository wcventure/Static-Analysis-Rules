static inline
void compute_images_mse_16bit(PSNRContext *s,
                        const uint8_t *main_data[4], const int main_linesizes[4],
                        const uint8_t *ref_data[4], const int ref_linesizes[4],
                        int w, int h, double mse[4])
{
    int i, c, j;

    for (c = 0; c < s->nb_components; c++) {
        const int outw = s->planewidth[c];
        const int outh = s->planeheight[c];
        const uint16_t *main_line = (uint16_t *)main_data[c];
        const uint16_t *ref_line = (uint16_t *)ref_data[c];
        const int ref_linesize = ref_linesizes[c] / 2;
        const int main_linesize = main_linesizes[c] / 2;
        uint64_t m = 0;

        for (i = 0; i < outh; i++) {
            for (j = 0; j < outw; j++)
                m += pow2(main_line[j] - ref_line[j]);
            ref_line += ref_linesize;
            main_line += main_linesize;
        }
        mse[c] = m / (double)(outw * outh);
    }
}
