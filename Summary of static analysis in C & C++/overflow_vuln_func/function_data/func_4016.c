static int update_wrap_reference(AVFormatContext *s, AVStream *st, int stream_index)
{
    if (s->correct_ts_overflow && st->pts_wrap_bits != 64 &&
        st->pts_wrap_reference == AV_NOPTS_VALUE && st->first_dts != AV_NOPTS_VALUE) {
        int i;

        // reference time stamp should be 60 s before first time stamp
        int64_t pts_wrap_reference = st->first_dts - av_rescale(60, st->time_base.den, st->time_base.num);
        // if first time stamp is not more than 1/8 and 60s before the wrap point, subtract rather than add wrap offset
        int pts_wrap_behavior = (st->first_dts < (1LL<<st->pts_wrap_bits) - (1LL<<st->pts_wrap_bits-3)) ||
            (st->first_dts < (1LL<<st->pts_wrap_bits) - av_rescale(60, st->time_base.den, st->time_base.num)) ?
            AV_PTS_WRAP_ADD_OFFSET : AV_PTS_WRAP_SUB_OFFSET;

        AVProgram *first_program = av_find_program_from_stream(s, NULL, stream_index);

        if (!first_program) {
            int default_stream_index = av_find_default_stream_index(s);
            if (s->streams[default_stream_index]->pts_wrap_reference == AV_NOPTS_VALUE) {
                for (i=0; i<s->nb_streams; i++) {
                    s->streams[i]->pts_wrap_reference = pts_wrap_reference;
                    s->streams[i]->pts_wrap_behavior = pts_wrap_behavior;
                }
            }
            else {
                st->pts_wrap_reference = s->streams[default_stream_index]->pts_wrap_reference;
                st->pts_wrap_behavior = s->streams[default_stream_index]->pts_wrap_behavior;
            }
        }
        else {
            AVProgram *program = first_program;
            while (program) {
                if (program->pts_wrap_reference != AV_NOPTS_VALUE) {
                    pts_wrap_reference = program->pts_wrap_reference;
                    pts_wrap_behavior = program->pts_wrap_behavior;
                    break;
                }
                program = av_find_program_from_stream(s, program, stream_index);
            }

            // update every program with differing pts_wrap_reference
            program = first_program;
            while(program) {
                if (program->pts_wrap_reference != pts_wrap_reference) {
                    for (i=0; i<program->nb_stream_indexes; i++) {
                        s->streams[program->stream_index[i]]->pts_wrap_reference = pts_wrap_reference;
                        s->streams[program->stream_index[i]]->pts_wrap_behavior = pts_wrap_behavior;
                    }

                    program->pts_wrap_reference = pts_wrap_reference;
                    program->pts_wrap_behavior = pts_wrap_behavior;
                }
                program = av_find_program_from_stream(s, program, stream_index);
            }
        }
        return 1;
    }
    return 0;
}
