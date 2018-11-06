static int mxf_parse_index(MXFContext *mxf, int track_id, AVStream *st)
{
    int64_t accumulated_offset = 0;
    int j, k, ret, nb_sorted_segments;
    MXFIndexTableSegment **sorted_segments;
    int n_delta = track_id - 1;  /

    if (track_id < 1) {
        av_log(mxf->fc, AV_LOG_ERROR, "TrackID not positive: %i\n", track_id);
        return AVERROR_INVALIDDATA;
    }

    if ((ret = mxf_get_sorted_table_segments(mxf, &nb_sorted_segments, &sorted_segments)))
        return ret;

    for (j = 0; j < nb_sorted_segments; j++) {
        int duration, sample_duration = 1, last_sample_size = 0;
        int64_t segment_size;
        MXFIndexTableSegment *tableseg = sorted_segments[j];

        /
        if (j > 0 && tableseg->body_sid != sorted_segments[j-1]->body_sid)
            accumulated_offset = 0;

        if (n_delta >= tableseg->nb_delta_entries && st->index != 0)
            continue;
        duration = tableseg->index_duration > 0 ? tableseg->index_duration :
            st->duration - st->nb_index_entries;
        segment_size = tableseg->edit_unit_byte_count * duration;
        /
        if (tableseg->edit_unit_byte_count && tableseg->edit_unit_byte_count < 32
            && !tableseg->index_duration) {
            /
            sample_duration = 8192;
            last_sample_size = (duration % sample_duration) * tableseg->edit_unit_byte_count;
            tableseg->edit_unit_byte_count *= sample_duration;
            duration /= sample_duration;
            if (last_sample_size) duration++;
        }

        for (k = 0; k < duration; k++) {
            int64_t pos;
            int size, flags = 0;

            if (k < tableseg->nb_index_entries) {
                pos = tableseg->stream_offset_entries[k];
                if (n_delta < tableseg->nb_delta_entries) {
                    if (n_delta < tableseg->nb_delta_entries - 1) {
                        size =
                            tableseg->slice_offset_entries[k][tableseg->slice[n_delta+1]-1] +
                            tableseg->element_delta[n_delta+1] -
                            tableseg->element_delta[n_delta];
                        if (tableseg->slice[n_delta] > 0)
                            size -= tableseg->slice_offset_entries[k][tableseg->slice[n_delta]-1];
                    } else if (k < duration - 1) {
                        size = tableseg->stream_offset_entries[k+1] -
                            tableseg->stream_offset_entries[k] -
                            tableseg->slice_offset_entries[k][tableseg->slice[tableseg->nb_delta_entries-1]-1] -
                            tableseg->element_delta[tableseg->nb_delta_entries-1];
                    } else
                        size = 0;
                    if (tableseg->slice[n_delta] > 0)
                        pos += tableseg->slice_offset_entries[k][tableseg->slice[n_delta]-1];
                    pos += tableseg->element_delta[n_delta];
                } else
                    size = 0;
                flags = !(tableseg->flag_entries[k] & 0x30) ? AVINDEX_KEYFRAME : 0;
            } else {
                pos = (int64_t)k * tableseg->edit_unit_byte_count + accumulated_offset;
                if (n_delta < tableseg->nb_delta_entries - 1)
                    size = tableseg->element_delta[n_delta+1] - tableseg->element_delta[n_delta];
                else {
                    /
                    if (last_sample_size && k == duration - 1)
                        size = last_sample_size;
                    else
                        size = tableseg->edit_unit_byte_count;
                    if (tableseg->nb_delta_entries)
                        size -= tableseg->element_delta[tableseg->nb_delta_entries-1];
                }
                if (n_delta < tableseg->nb_delta_entries)
                    pos += tableseg->element_delta[n_delta];
                flags = AVINDEX_KEYFRAME;
            }

            if (mxf_absolute_bodysid_offset(mxf, tableseg->body_sid, pos, &pos) < 0) {
                /
                break;
            }

            av_dlog(mxf->fc, "Stream %d IndexEntry %d TrackID %d Offset %"PRIx64" Timestamp %"PRId64"\n",
                    st->index, st->nb_index_entries, track_id, pos, sample_duration * st->nb_index_entries);

            if ((ret = av_add_index_entry(st, pos, sample_duration * st->nb_index_entries, size, 0, flags)) < 0)
                return ret;
        }
        accumulated_offset += segment_size;
    }

    av_free(sorted_segments);

    return 0;
}
