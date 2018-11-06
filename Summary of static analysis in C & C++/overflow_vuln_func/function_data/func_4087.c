static void adjust_frame_information(ChannelElement *cpe, int chans)
{
    int i, w, w2, g, ch;
    int maxsfb, cmaxsfb;
    IndividualChannelStream *ics;

    if (cpe->common_window) {
        ics = &cpe->ch[0].ics;
        for (w = 0; w < ics->num_windows; w += ics->group_len[w]) {
            for (w2 =  0; w2 < ics->group_len[w]; w2++) {
                int start = (w+w2) * 128;
                for (g = 0; g < ics->num_swb; g++) {
                    //apply Intensity stereo coeffs transformation
                    if (cpe->is_mask[w*16 + g]) {
                        int p = -1 + 2 * (cpe->ch[1].band_type[w*16+g] - 14);
                        float scale = cpe->ch[0].is_ener[w*16+g];
                        for (i = 0; i < ics->swb_sizes[g]; i++) {
                            cpe->ch[0].coeffs[start+i] = (cpe->ch[0].pcoeffs[start+i] + p*cpe->ch[1].pcoeffs[start+i]) * scale;
                            cpe->ch[1].coeffs[start+i] = 0.0f;
                        }
                    } else if (cpe->ms_mask[w*16 + g] &&
                               cpe->ch[0].band_type[w*16 + g] < NOISE_BT &&
                               cpe->ch[1].band_type[w*16 + g] < NOISE_BT) {
                        for (i = 0; i < ics->swb_sizes[g]; i++) {
                            cpe->ch[0].coeffs[start+i] = (cpe->ch[0].pcoeffs[start+i] + cpe->ch[1].pcoeffs[start+i]) * 0.5f;
                            cpe->ch[1].coeffs[start+i] = cpe->ch[0].coeffs[start+i] - cpe->ch[1].pcoeffs[start+i];
                        }
                    }
                    start += ics->swb_sizes[g];
                }
            }
        }
    }

    for (ch = 0; ch < chans; ch++) {
        IndividualChannelStream *ics = &cpe->ch[ch].ics;
        maxsfb = 0;
        cpe->ch[ch].pulse.num_pulse = 0;
        for (w = 0; w < ics->num_windows; w += ics->group_len[w]) {
            for (w2 =  0; w2 < ics->group_len[w]; w2++) {
                for (cmaxsfb = ics->num_swb; cmaxsfb > 0 && cpe->ch[ch].zeroes[w*16+cmaxsfb-1]; cmaxsfb--)
                    ;
                maxsfb = FFMAX(maxsfb, cmaxsfb);
            }
        }
        ics->max_sfb = maxsfb;

        //adjust zero bands for window groups
        for (w = 0; w < ics->num_windows; w += ics->group_len[w]) {
            for (g = 0; g < ics->max_sfb; g++) {
                i = 1;
                for (w2 = w; w2 < w + ics->group_len[w]; w2++) {
                    if (!cpe->ch[ch].zeroes[w2*16 + g]) {
                        i = 0;
                        break;
                    }
                }
                cpe->ch[ch].zeroes[w*16 + g] = i;
            }
        }
    }

    if (chans > 1 && cpe->common_window) {
        IndividualChannelStream *ics0 = &cpe->ch[0].ics;
        IndividualChannelStream *ics1 = &cpe->ch[1].ics;
        int msc = 0;
        ics0->max_sfb = FFMAX(ics0->max_sfb, ics1->max_sfb);
        ics1->max_sfb = ics0->max_sfb;
        for (w = 0; w < ics0->num_windows*16; w += 16)
            for (i = 0; i < ics0->max_sfb; i++)
                if (cpe->ms_mask[w+i])
                    msc++;
        if (msc == 0 || ics0->max_sfb == 0)
            cpe->ms_mode = 0;
        else
            cpe->ms_mode = msc < ics0->max_sfb * ics0->num_windows ? 1 : 2;
    }
}
