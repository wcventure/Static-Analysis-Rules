/// Dequantization and stereo decoding (14496-3 sp04 p203)
static void sbr_dequant(SpectralBandReplication *sbr, int id_aac)
{
    int k, e;
    int ch;

    if (id_aac == TYPE_CPE && sbr->bs_coupling) {
        int alpha      = sbr->data[0].bs_amp_res ?  2 :  1;
        int pan_offset = sbr->data[0].bs_amp_res ? 12 : 24;
        for (e = 1; e <= sbr->data[0].bs_num_env; e++) {
            for (k = 0; k < sbr->n[sbr->data[0].bs_freq_res[e]]; k++) {
                SoftFloat temp1, temp2, fac;

                temp1.exp = sbr->data[0].env_facs[e][k].mant * alpha + 14;
                if (temp1.exp & 1)
                  temp1.mant = 759250125;
                else
                  temp1.mant = 0x20000000;
                temp1.exp = (temp1.exp >> 1) + 1;

                temp2.exp = (pan_offset - sbr->data[1].env_facs[e][k].mant) * alpha;
                if (temp2.exp & 1)
                  temp2.mant = 759250125;
                else
                  temp2.mant = 0x20000000;
                temp2.exp = (temp2.exp >> 1) + 1;
                fac   = av_div_sf(temp1, av_add_sf(FLOAT_1, temp2));
                sbr->data[0].env_facs[e][k] = fac;
                sbr->data[1].env_facs[e][k] = av_mul_sf(fac, temp2);
            }
        }
        for (e = 1; e <= sbr->data[0].bs_num_noise; e++) {
            for (k = 0; k < sbr->n_q; k++) {
                SoftFloat temp1, temp2, fac;

                temp1.exp = NOISE_FLOOR_OFFSET - \
                    sbr->data[0].noise_facs[e][k].mant + 2;
                temp1.mant = 0x20000000;
                temp2.exp = 12 - sbr->data[1].noise_facs[e][k].mant + 1;
                temp2.mant = 0x20000000;
                fac   = av_div_sf(temp1, av_add_sf(FLOAT_1, temp2));
                sbr->data[0].noise_facs[e][k] = fac;
                sbr->data[1].noise_facs[e][k] = av_mul_sf(fac, temp2);
            }
        }
    } else { // SCE or one non-coupled CPE
        for (ch = 0; ch < (id_aac == TYPE_CPE) + 1; ch++) {
            int alpha = sbr->data[ch].bs_amp_res ? 2 : 1;
            for (e = 1; e <= sbr->data[ch].bs_num_env; e++)
                for (k = 0; k < sbr->n[sbr->data[ch].bs_freq_res[e]]; k++){
                    SoftFloat temp1;

                    temp1.exp = alpha * sbr->data[ch].env_facs[e][k].mant + 12;
                    if (temp1.exp & 1)
                        temp1.mant = 759250125;
                    else
                        temp1.mant = 0x20000000;
                    temp1.exp = (temp1.exp >> 1) + 1;

                    sbr->data[ch].env_facs[e][k] = temp1;
                }
            for (e = 1; e <= sbr->data[ch].bs_num_noise; e++)
                for (k = 0; k < sbr->n_q; k++){
                    sbr->data[ch].noise_facs[e][k].exp = NOISE_FLOOR_OFFSET - \
                        sbr->data[ch].noise_facs[e][k].mant + 1;
                    sbr->data[ch].noise_facs[e][k].mant = 0x20000000;
                }
        }
    }
}
