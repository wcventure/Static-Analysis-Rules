static void apply_dependent_coupling_fixed(AACContext *ac,
                                     SingleChannelElement *target,
                                     ChannelElement *cce, int index)
{
    IndividualChannelStream *ics = &cce->ch[0].ics;
    const uint16_t *offsets = ics->swb_offset;
    int *dest = target->coeffs;
    const int *src = cce->ch[0].coeffs;
    int g, i, group, k, idx = 0;
    if (ac->oc[1].m4ac.object_type == AOT_AAC_LTP) {
        av_log(ac->avctx, AV_LOG_ERROR,
               "Dependent coupling is not supported together with LTP\n");
        return;
    }
    for (g = 0; g < ics->num_window_groups; g++) {
        for (i = 0; i < ics->max_sfb; i++, idx++) {
            if (cce->ch[0].band_type[idx] != ZERO_BT) {
                const int gain = cce->coup.gain[index][idx];
                int shift, round, c, tmp;

                if (gain < 0) {
                    c = -cce_scale_fixed[-gain & 7];
                    shift = (-gain-1024) >> 3;
                }
                else {
                    c = cce_scale_fixed[gain & 7];
                    shift = (gain-1024) >> 3;
                }

                if (shift < -31) {
                    // Nothing to do
                } else if (shift < 0) {
                    shift = -shift;
                    round = 1 << (shift - 1);

                    for (group = 0; group < ics->group_len[g]; group++) {
                        for (k = offsets[i]; k < offsets[i + 1]; k++) {
                            tmp = (int)(((int64_t)src[group * 128 + k] * c + \
                                       (int64_t)0x1000000000) >> 37);
                            dest[group * 128 + k] += (tmp + round) >> shift;
                        }
                    }
                }
                else {
                    for (group = 0; group < ics->group_len[g]; group++) {
                        for (k = offsets[i]; k < offsets[i + 1]; k++) {
                            tmp = (int)(((int64_t)src[group * 128 + k] * c + \
                                        (int64_t)0x1000000000) >> 37);
                            dest[group * 128 + k] += tmp * (1 << shift);
                        }
                    }
                }
            }
        }
        dest += ics->group_len[g] * 128;
        src  += ics->group_len[g] * 128;
    }
}
