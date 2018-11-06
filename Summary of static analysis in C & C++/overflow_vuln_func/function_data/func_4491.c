static AVIOContext * wtvfile_open2(AVFormatContext *s, const uint8_t *buf, int buf_size, const uint8_t *filename, int filename_size)
{
    const uint8_t *buf_end = buf + buf_size;

    while(buf + 48 <= buf_end) {
        int dir_length, name_size, first_sector, depth;
        uint64_t file_length;
        const uint8_t *name;
        if (ff_guidcmp(buf, dir_entry_guid)) {
            av_log(s, AV_LOG_ERROR, "unknown guid "FF_PRI_GUID", expected dir_entry_guid; "
                   "remaining directory entries ignored\n", FF_ARG_GUID(buf));
            break;
        }
        dir_length  = AV_RL16(buf + 16);
        file_length = AV_RL64(buf + 24);
        name_size   = 2 * AV_RL32(buf + 32);
        if (buf + 48 + name_size > buf_end) {
            av_log(s, AV_LOG_ERROR, "filename exceeds buffer size; remaining directory entries ignored\n");
            break;
        }
        first_sector = AV_RL32(buf + 40 + name_size);
        depth        = AV_RL32(buf + 44 + name_size);

        /
        name = buf + 40;
        if (name_size >= filename_size &&
            !memcmp(name, filename, filename_size) &&
            (name_size < filename_size + 2 || !AV_RN16(name + filename_size)))
            return wtvfile_open_sector(first_sector, file_length, depth, s);

        buf += dir_length;
    }
    return 0;
}
