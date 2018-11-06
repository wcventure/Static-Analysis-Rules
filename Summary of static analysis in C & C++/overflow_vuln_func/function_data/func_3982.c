static void mov_fix_index(MOVContext *mov, AVStream *st)
{
    MOVStreamContext *msc = st->priv_data;
    AVIndexEntry *e_old = st->index_entries;
    int nb_old = st->nb_index_entries;
    const AVIndexEntry *e_old_end = e_old + nb_old;
    const AVIndexEntry *current = NULL;
    MOVStts *ctts_data_old = msc->ctts_data;
    int64_t ctts_index_old = 0;
    int64_t ctts_sample_old = 0;
    int64_t ctts_count_old = msc->ctts_count;
    int64_t edit_list_media_time = 0;
    int64_t edit_list_duration = 0;
    int64_t frame_duration = 0;
    int64_t edit_list_dts_counter = 0;
    int64_t edit_list_dts_entry_end = 0;
    int64_t edit_list_start_ctts_sample = 0;
    int64_t curr_cts;
    int64_t edit_list_index = 0;
    int64_t index;
    int64_t index_ctts_count;
    int flags;
    unsigned int ctts_allocated_size = 0;
    int64_t start_dts = 0;
    int64_t edit_list_media_time_dts = 0;
    int64_t edit_list_start_encountered = 0;
    int64_t search_timestamp = 0;
    int64_t* frame_duration_buffer = NULL;
    int num_discarded_begin = 0;
    int first_non_zero_audio_edit = -1;
    int packet_skip_samples = 0;

    if (!msc->elst_data || msc->elst_count <= 0) {
        return;
    }
    // Clean AVStream from traces of old index
    st->index_entries = NULL;
    st->index_entries_allocated_size = 0;
    st->nb_index_entries = 0;

    // Clean ctts fields of MOVStreamContext
    msc->ctts_data = NULL;
    msc->ctts_count = 0;
    msc->ctts_index = 0;
    msc->ctts_sample = 0;

    // If the dts_shift is positive (in case of negative ctts values in mov),
    // then negate the DTS by dts_shift
    if (msc->dts_shift > 0)
        edit_list_dts_entry_end -= msc->dts_shift;

    // Offset the DTS by ctts[0] to make the PTS of the first frame 0
    if (ctts_data_old && ctts_count_old > 0) {
        edit_list_dts_entry_end -= ctts_data_old[0].duration;
        av_log(mov->fc, AV_LOG_DEBUG, "Offset DTS by ctts[%d].duration: %d\n", 0, ctts_data_old[0].duration);
    }

    start_dts = edit_list_dts_entry_end;

    while (get_edit_list_entry(mov, msc, edit_list_index, &edit_list_media_time,
                               &edit_list_duration, mov->time_scale)) {
        av_log(mov->fc, AV_LOG_DEBUG, "Processing st: %d, edit list %"PRId64" - media time: %"PRId64", duration: %"PRId64"\n",
               st->index, edit_list_index, edit_list_media_time, edit_list_duration);
        edit_list_index++;
        edit_list_dts_counter = edit_list_dts_entry_end;
        edit_list_dts_entry_end += edit_list_duration;
        num_discarded_begin = 0;
        if (edit_list_media_time == -1) {
            continue;
        }

        // If we encounter a non-negative edit list reset the skip_samples/start_pad fields and set them
        // according to the edit list below.
        if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (first_non_zero_audio_edit < 0) {
                first_non_zero_audio_edit = 1;
            } else {
                first_non_zero_audio_edit = 0;
            }

            if (first_non_zero_audio_edit > 0)
                st->skip_samples = msc->start_pad = 0;
        }

        //find closest previous key frame
        edit_list_media_time_dts = edit_list_media_time;
        if (msc->dts_shift > 0) {
            edit_list_media_time_dts -= msc->dts_shift;
        }

        // While reordering frame index according to edit list we must handle properly
        // the scenario when edit list entry starts from none key frame.
        // We find closest previous key frame and preserve it and consequent frames in index.
        // All frames which are outside edit list entry time boundaries will be dropped after decoding.
        search_timestamp = edit_list_media_time_dts;
        if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            // Audio decoders like AAC need need a decoder delay samples previous to the current sample,
            // to correctly decode this frame. Hence for audio we seek to a frame 1 sec. before the
            // edit_list_media_time to cover the decoder delay.
            search_timestamp = FFMAX(search_timestamp - msc->time_scale, e_old[0].timestamp);
        }

        index = find_prev_closest_keyframe_index(st, e_old, nb_old, search_timestamp, 0);
        if (index == -1) {
            av_log(mov->fc, AV_LOG_ERROR, "Missing key frame while reordering index according to edit list\n");
            continue;
        }
        current = e_old + index;

        ctts_index_old = 0;
        ctts_sample_old = 0;

        // set ctts_index properly for the found key frame
        for (index_ctts_count = 0; index_ctts_count < index; index_ctts_count++) {
            if (ctts_data_old && ctts_index_old < ctts_count_old) {
                ctts_sample_old++;
                if (ctts_data_old[ctts_index_old].count == ctts_sample_old) {
                    ctts_index_old++;
                    ctts_sample_old = 0;
                }
            }
        }

        edit_list_start_ctts_sample = ctts_sample_old;

        // Iterate over index and arrange it according to edit list
        edit_list_start_encountered = 0;
        for (; current < e_old_end; current++, index++) {
            // check  if frame outside edit list mark it for discard
            frame_duration = (current + 1 <  e_old_end) ?
                             ((current + 1)->timestamp - current->timestamp) : edit_list_duration;

            flags = current->flags;

            // frames (pts) before or after edit list
            curr_cts = current->timestamp + msc->dts_shift;

            if (ctts_data_old && ctts_index_old < ctts_count_old) {
                av_log(mov->fc, AV_LOG_DEBUG, "shifted frame pts, curr_cts: %"PRId64" @ %"PRId64", ctts: %d, ctts_count: %"PRId64"\n",
                       curr_cts, ctts_index_old, ctts_data_old[ctts_index_old].duration, ctts_count_old);
                curr_cts += ctts_data_old[ctts_index_old].duration;
                ctts_sample_old++;
                if (ctts_sample_old == ctts_data_old[ctts_index_old].count) {
                    if (add_ctts_entry(&msc->ctts_data, &msc->ctts_count,
                                       &ctts_allocated_size,
                                       ctts_data_old[ctts_index_old].count - edit_list_start_ctts_sample,
                                       ctts_data_old[ctts_index_old].duration) == -1) {
                        av_log(mov->fc, AV_LOG_ERROR, "Cannot add CTTS entry %"PRId64" - {%"PRId64", %d}\n",
                               ctts_index_old,
                               ctts_data_old[ctts_index_old].count - edit_list_start_ctts_sample,
                               ctts_data_old[ctts_index_old].duration);
                        break;
                    }
                    ctts_index_old++;
                    ctts_sample_old = 0;
                    edit_list_start_ctts_sample = 0;
                }
            }

            if (curr_cts < edit_list_media_time || curr_cts >= (edit_list_duration + edit_list_media_time)) {
                if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && st->codecpar->codec_id != AV_CODEC_ID_VORBIS &&
                    curr_cts < edit_list_media_time && curr_cts + frame_duration > edit_list_media_time &&
                    first_non_zero_audio_edit > 0) {
                    packet_skip_samples = edit_list_media_time - curr_cts;
                    st->skip_samples += packet_skip_samples;

                    // Shift the index entry timestamp by packet_skip_samples to be correct.
                    edit_list_dts_counter -= packet_skip_samples;
                    if (edit_list_start_encountered == 0)  {
                        edit_list_start_encountered = 1;
                        // Make timestamps strictly monotonically increasing for audio, by rewriting timestamps for
                        // discarded packets.
                        if (frame_duration_buffer) {
                            fix_index_entry_timestamps(st, st->nb_index_entries, edit_list_dts_counter,
                                                       frame_duration_buffer, num_discarded_begin);
                            av_freep(&frame_duration_buffer);
                        }
                    }

                    av_log(mov->fc, AV_LOG_DEBUG, "skip %d audio samples from curr_cts: %"PRId64"\n", packet_skip_samples, curr_cts);
                } else {
                    flags |= AVINDEX_DISCARD_FRAME;
                    av_log(mov->fc, AV_LOG_DEBUG, "drop a frame at curr_cts: %"PRId64" @ %"PRId64"\n", curr_cts, index);

                    if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && edit_list_start_encountered == 0) {
                        num_discarded_begin++;
                        frame_duration_buffer = av_realloc(frame_duration_buffer,
                                                           num_discarded_begin * sizeof(int64_t));
                        if (!frame_duration_buffer) {
                            av_log(mov->fc, AV_LOG_ERROR, "Cannot reallocate frame duration buffer\n");
                            break;
                        }
                        frame_duration_buffer[num_discarded_begin - 1] = frame_duration;

                        // Increment skip_samples for the first non-zero audio edit list
                        if (first_non_zero_audio_edit > 0 && st->codecpar->codec_id != AV_CODEC_ID_VORBIS) {
                            st->skip_samples += frame_duration;
                            msc->start_pad = st->skip_samples;
                        }
                    }
                }
            } else if (edit_list_start_encountered == 0) {
                edit_list_start_encountered = 1;
                // Make timestamps strictly monotonically increasing for audio, by rewriting timestamps for
                // discarded packets.
                if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && frame_duration_buffer) {
                    fix_index_entry_timestamps(st, st->nb_index_entries, edit_list_dts_counter,
                                               frame_duration_buffer, num_discarded_begin);
                    av_freep(&frame_duration_buffer);
                }
            }

            if (add_index_entry(st, current->pos, edit_list_dts_counter, current->size,
                                current->min_distance, flags) == -1) {
                av_log(mov->fc, AV_LOG_ERROR, "Cannot add index entry\n");
                break;
            }

            // Only start incrementing DTS in frame_duration amounts, when we encounter a frame in edit list.
            if (edit_list_start_encountered > 0) {
                edit_list_dts_counter = edit_list_dts_counter + frame_duration;
            }

            // Break when found first key frame after edit entry completion
            if (((curr_cts + frame_duration) >= (edit_list_duration + edit_list_media_time)) &&
                ((flags & AVINDEX_KEYFRAME) || ((st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)))) {

                if (ctts_data_old && ctts_sample_old != 0) {
                    if (add_ctts_entry(&msc->ctts_data, &msc->ctts_count,
                                       &ctts_allocated_size,
                                       ctts_sample_old - edit_list_start_ctts_sample,
                                       ctts_data_old[ctts_index_old].duration) == -1) {
                        av_log(mov->fc, AV_LOG_ERROR, "Cannot add CTTS entry %"PRId64" - {%"PRId64", %d}\n",
                               ctts_index_old, ctts_sample_old - edit_list_start_ctts_sample,
                               ctts_data_old[ctts_index_old].duration);
                        break;
                    }
                }
                break;
            }
        }
    }
    // Update av stream length
    st->duration = edit_list_dts_entry_end - start_dts;

    // Free the old index and the old CTTS structures
    av_free(e_old);
    av_free(ctts_data_old);
}
