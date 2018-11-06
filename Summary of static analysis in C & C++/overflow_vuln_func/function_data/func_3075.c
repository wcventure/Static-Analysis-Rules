/// Dequantization and stereo decoding (14496-3 sp04 p203)
static void sbr_dequant(SpectralBandReplication *sbr, int id_aac)
{
    int k, e;
    int ch;

    if (id_aac == TYPE_CPE && sbr->bs_coupling) {
        float alpha      = sbr->data[0].bs_amp_res ?  1.0f :  0.5f;
        float pan_offset = sbr->data[0].bs_amp_res ? 12.0f : 24.0f;
        for (e = 1; e <= sbr->data[0].bs_num_env; e++) {
            for (k = 0; k < sbr->n[sbr->data[0].bs_freq_res[e]]; k++) {
                float temp1 = exp2f(sbr->data[0].env_facs[e][k] * alpha + 7.0f);
                float temp2 = exp2f((pan_offset - sbr->data[1].env_facs[e][k]) * alpha);
                float fac   = temp1 / (1.0f + temp2);
                sbr->data[0].env_facs[e][k] = fac;
                sbr->data[1].env_facs[e][k] = fac * temp2;
            }
        }
        for (e = 1; e <= sbr->data[0].bs_num_noise; e++) {
            for (k = 0; k < sbr->n_q; k++) {
                float temp1 = exp2f(NOISE_FLOOR_OFFSET - sbr->data[0].noise_facs[e][k] + 1);
                float temp2 = exp2f(12 - sbr->data[1].noise_facs[e][k]);
                float fac   = temp1 / (1.0f + temp2);
                sbr->data[0].noise_facs[e][k] = fac;
                sbr->data[1].noise_facs[e][k] = fac * temp2;
            }
        }
    } else { // SCE or one non-coupled CPE
        for (ch = 0; ch < (id_aac == TYPE_CPE) + 1; ch++) {
            float alpha = sbr->data[ch].bs_amp_res ? 1.0f : 0.5f;
            for (e = 1; e <= sbr->data[ch].bs_num_env; e++)
                for (k = 0; k < sbr->n[sbr->data[ch].bs_freq_res[e]]; k++)
                    sbr->data[ch].env_facs[e][k] =
                        exp2f(alpha * sbr->data[ch].env_facs[e][k] + 6.0f);
            for (e = 1; e <= sbr->data[ch].bs_num_noise; e++)
                for (k = 0; k < sbr->n_q; k++)
                    sbr->data[ch].noise_facs[e][k] =
                        exp2f(NOISE_FLOOR_OFFSET - sbr->data[ch].noise_facs[e][k]);
        }
    }
}
