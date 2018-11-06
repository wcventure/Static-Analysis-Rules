static int64_t wrap_timestamp(AVStream *st, int64_t timestamp)
{
    if (st->pts_wrap_behavior != AV_PTS_WRAP_IGNORE && st->pts_wrap_bits < 64 &&
        st->pts_wrap_reference != AV_NOPTS_VALUE && timestamp != AV_NOPTS_VALUE) {
        if (st->pts_wrap_behavior == AV_PTS_WRAP_ADD_OFFSET &&
            timestamp < st->pts_wrap_reference)
            return timestamp + (1LL<<st->pts_wrap_bits);
        else if (st->pts_wrap_behavior == AV_PTS_WRAP_SUB_OFFSET &&
            timestamp >= st->pts_wrap_reference)
            return timestamp - (1LL<<st->pts_wrap_bits);
    }
    return timestamp;
}
