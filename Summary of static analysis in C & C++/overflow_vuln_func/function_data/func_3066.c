static av_cold int che_configure(AACContext *ac,
                                 enum ChannelPosition che_pos,
                                 int type, int id, int *channels)
{
    if (*channels >= MAX_CHANNELS)
        return AVERROR_INVALIDDATA;
    if (che_pos) {
        if (!ac->che[type][id]) {
            if (!(ac->che[type][id] = av_mallocz(sizeof(ChannelElement))))
                return AVERROR(ENOMEM);
            ff_aac_sbr_ctx_init(ac, &ac->che[type][id]->sbr);
        }
        if (type != TYPE_CCE) {
            ac->output_element[(*channels)++] = &ac->che[type][id]->ch[0];
            if (type == TYPE_CPE ||
                (type == TYPE_SCE && ac->oc[1].m4ac.ps == 1)) {
                ac->output_element[(*channels)++] = &ac->che[type][id]->ch[1];
            }
        }
    } else {
        if (ac->che[type][id])
            ff_aac_sbr_ctx_close(&ac->che[type][id]->sbr);
        av_freep(&ac->che[type][id]);
    }
    return 0;
}
