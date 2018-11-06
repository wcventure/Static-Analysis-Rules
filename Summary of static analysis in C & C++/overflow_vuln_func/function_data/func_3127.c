static void celt_denormalize(CeltFrame *f, CeltBlock *block, float *data)
{
    int i, j;

    for (i = f->start_band; i < f->end_band; i++) {
        float *dst = data + (ff_celt_freq_bands[i] << f->size);
        float norm = exp2f(block->energy[i] + ff_celt_mean_energy[i]);

        for (j = 0; j < ff_celt_freq_range[i] << f->size; j++)
            dst[j] *= norm;
    }
}
