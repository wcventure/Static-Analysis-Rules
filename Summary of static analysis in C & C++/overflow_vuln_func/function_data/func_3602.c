static int mxf_edit_unit_absolute_offset(MXFContext *mxf, MXFIndexTable *index_table, int64_t edit_unit, int64_t *edit_unit_out, int64_t *offset_out, int nag)
{
    int i;
    int offset_temp = 0;

    for (i = 0; i < index_table->nb_segments; i++) {
        MXFIndexTableSegment *s = index_table->segments[i];

        edit_unit = FFMAX(edit_unit, s->index_start_position);  /

        if (edit_unit < s->index_start_position + s->index_duration) {
            int64_t index = edit_unit - s->index_start_position;

            if (s->edit_unit_byte_count)
                offset_temp += s->edit_unit_byte_count * index;
            else if (s->nb_index_entries) {
                if (s->nb_index_entries == 2 * s->index_duration + 1)
                    index *= 2;     /

                if (index < 0 || index > s->nb_index_entries) {
                    av_log(mxf->fc, AV_LOG_ERROR, "IndexSID %i segment at %"PRId64" IndexEntryArray too small\n",
                           index_table->index_sid, s->index_start_position);
                    return AVERROR_INVALIDDATA;
                }

                offset_temp = s->stream_offset_entries[index];
            } else {
                av_log(mxf->fc, AV_LOG_ERROR, "IndexSID %i segment at %"PRId64" missing EditUnitByteCount and IndexEntryArray\n",
                       index_table->index_sid, s->index_start_position);
                return AVERROR_INVALIDDATA;
            }

            if (edit_unit_out)
                *edit_unit_out = edit_unit;

            return mxf_absolute_bodysid_offset(mxf, index_table->body_sid, offset_temp, offset_out);
        } else {
            /
            offset_temp += s->edit_unit_byte_count * s->index_duration;
        }
    }

    if (nag)
        av_log(mxf->fc, AV_LOG_ERROR, "failed to map EditUnit %"PRId64" in IndexSID %i to an offset\n", edit_unit, index_table->index_sid);

    return AVERROR_INVALIDDATA;
}
