static void get_tag(AVFormatContext *s, const char *key, int type, int len, int type2_size)
{
    char *value;
    int64_t off = avio_tell(s->pb);

    if ((unsigned)len >= (UINT_MAX - 1) / 2)
        return;

    value = av_malloc(2 * len + 1);
    if (!value)
        goto finish;

    if (type == 0) {         // UTF16-LE
        avio_get_str16le(s->pb, len, value, 2 * len + 1);
    } else if (type == -1) { // ASCII
        avio_read(s->pb, value, len);
        value[len]=0;
    } else if (type == 1) {  // byte array
        if (!strcmp(key, "WM/Picture")) { // handle cover art
            asf_read_picture(s, len);
        } else if (!strcmp(key, "ID3")) { // handle ID3 tag
            get_id3_tag(s, len);
        } else {
            av_log(s, AV_LOG_VERBOSE, "Unsupported byte array in tag %s.\n", key);
        }
        goto finish;
    } else if (type > 1 && type <= 5) {  // boolean or DWORD or QWORD or WORD
        uint64_t num = get_value(s->pb, type, type2_size);
        snprintf(value, len, "%"PRIu64, num);
    } else if (type == 6) { // (don't) handle GUID
        av_log(s, AV_LOG_DEBUG, "Unsupported GUID value in tag %s.\n", key);
        goto finish;
    } else {
        av_log(s, AV_LOG_DEBUG,
               "Unsupported value type %d in tag %s.\n", type, key);
        goto finish;
    }
    if (*value)
        av_dict_set(&s->metadata, key, value, 0);

finish:
    av_freep(&value);
    avio_seek(s->pb, off + len, SEEK_SET);
}
