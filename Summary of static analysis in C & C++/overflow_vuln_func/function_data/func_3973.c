static void decode_band_structure(GetBitContext *gbc, int blk, int eac3,
                                  int ecpl, int start_subband, int end_subband,
                                  const uint8_t *default_band_struct,
                                  int *num_bands, uint8_t *band_sizes)
{
    int subbnd, bnd, n_subbands, n_bands=0;
    uint8_t bnd_sz[22];
    uint8_t coded_band_struct[22];
    const uint8_t *band_struct;

    n_subbands = end_subband - start_subband;

    /
    if (!eac3 || get_bits1(gbc)) {
        for (subbnd = 0; subbnd < n_subbands - 1; subbnd++) {
            coded_band_struct[subbnd] = get_bits1(gbc);
        }
        band_struct = coded_band_struct;
    } else if (!blk) {
        band_struct = &default_band_struct[start_subband+1];
    } else {
        /
        return;
    }

    /
    if (num_bands || band_sizes ) {
        n_bands = n_subbands;
        bnd_sz[0] = ecpl ? 6 : 12;
        for (bnd = 0, subbnd = 1; subbnd < n_subbands; subbnd++) {
            int subbnd_size = (ecpl && subbnd < 4) ? 6 : 12;
            if (band_struct[subbnd - 1]) {
                n_bands--;
                bnd_sz[bnd] += subbnd_size;
            } else {
                bnd_sz[++bnd] = subbnd_size;
            }
        }
    }

    /
    if (num_bands)
        *num_bands = n_bands;
    if (band_sizes)
        memcpy(band_sizes, bnd_sz, n_bands);
}
