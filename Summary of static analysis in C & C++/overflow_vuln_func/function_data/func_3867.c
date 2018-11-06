static void mxf_packet_timestamps(MXFContext *mxf, AVPacket *pkt)
{
    int64_t last_ofs = -1, next_ofs;
    MXFIndexTable *t = &mxf->index_tables[0];

    /
    if (mxf->nb_index_tables <= 0)
        return;

    /
    for (;;) {
        if (mxf_edit_unit_absolute_offset(mxf, t, mxf->current_edit_unit + 1, NULL, &next_ofs, 0) < 0)
            break;

        if (next_ofs <= last_ofs) {
            /
            av_log(mxf->fc, AV_LOG_ERROR,
                   "next_ofs didn't change. not deriving packet timestamps\n");
            return;
        }

        if (next_ofs > pkt->pos)
            break;

        last_ofs = next_ofs;
        mxf->current_edit_unit++;
    }

    if (mxf->current_edit_unit >= t->nb_ptses)
        return;

    pkt->dts = mxf->current_edit_unit + t->first_dts;
    pkt->pts = t->ptses[mxf->current_edit_unit];
}
