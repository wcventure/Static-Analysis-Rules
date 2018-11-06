static int mov_read_dref(MOVContext *c, ByteIOContext *pb, MOV_atom_t atom)
{
    AVStream *st = c->fc->streams[c->fc->nb_streams-1];
    MOVStreamContext *sc = st->priv_data;
    int entries, i, j;

    get_be32(pb); // version + flags
    entries = get_be32(pb);
    if (entries >= UINT_MAX / sizeof(*sc->drefs))
        return -1;
    sc->drefs_count = entries;
    sc->drefs = av_mallocz(entries * sizeof(*sc->drefs));

    for (i = 0; i < sc->drefs_count; i++) {
        MOV_dref_t *dref = &sc->drefs[i];
        uint32_t size = get_be32(pb);
        offset_t next = url_ftell(pb) + size - 4;

        dref->type = get_le32(pb);
        get_be32(pb); // version + flags
        dprintf(c->fc, "type %.4s size %d\n", (char*)&dref->type, size);

        if (dref->type == MKTAG('a','l','i','s') && size > 150) {
            /
            uint16_t volume_len, len;
            char volume[28];
            int16_t type;

            url_fskip(pb, 10);

            volume_len = get_byte(pb);
            volume_len = FFMIN(volume_len, 27);
            get_buffer(pb, volume, 27);
            volume[volume_len] = 0;
            av_log(c->fc, AV_LOG_DEBUG, "volume %s, len %d\n", volume, volume_len);

            url_fskip(pb, 112);

            for (type = 0; type != -1 && url_ftell(pb) < next; ) {
                type = get_be16(pb);
                len = get_be16(pb);
                av_log(c->fc, AV_LOG_DEBUG, "type %d, len %d\n", type, len);
                if (len&1)
                    len += 1;
                if (type == 2) { // absolute path
                    dref->path = av_mallocz(len+1);
                    get_buffer(pb, dref->path, len);
                    if (!strncmp(dref->path, volume, volume_len)) {
                        len -= volume_len;
                        memmove(dref->path, dref->path+volume_len, len);
                        dref->path[len] = 0;
                    }
                    for (j = 0; j < len; j++)
                        if (dref->path[j] == ':')
                            dref->path[j] = '/';
                    av_log(c->fc, AV_LOG_DEBUG, "path %s\n", dref->path);
                } else
                    url_fskip(pb, len);
            }
        }
        url_fseek(pb, next, SEEK_SET);
    }
    return 0;
}
