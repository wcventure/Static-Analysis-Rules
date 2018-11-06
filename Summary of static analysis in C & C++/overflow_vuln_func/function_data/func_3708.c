static int mov_read_stsz(MOVContext *c, ByteIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    MOVStreamContext *sc;
    unsigned int i, entries, sample_size, field_size, num_bytes;
    GetBitContext gb;
    unsigned char* buf;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];
    sc = st->priv_data;

    get_byte(pb); /
    get_be24(pb); /

    if (atom.type == MKTAG('s','t','s','z')) {
        sample_size = get_be32(pb);
        if (!sc->sample_size) /
            sc->sample_size = sample_size;
        field_size = 32;
    } else {
        sample_size = 0;
        get_be24(pb); /
        field_size = get_byte(pb);
    }
    entries = get_be32(pb);

    dprintf(c->fc, "sample_size = %d sample_count = %d\n", sc->sample_size, entries);

    sc->sample_count = entries;
    if (sample_size)
        return 0;

    if (field_size != 4 && field_size != 8 && field_size != 16 && field_size != 32) {
        av_log(c->fc, AV_LOG_ERROR, "Invalid sample field size %d\n", field_size);
        return -1;
    }

    if(entries >= UINT_MAX / sizeof(int))
        return -1;
    sc->sample_sizes = av_malloc(entries * sizeof(int));
    if (!sc->sample_sizes)
        return AVERROR(ENOMEM);

    num_bytes = (entries*field_size+4)>>3;

    buf = av_malloc(num_bytes+FF_INPUT_BUFFER_PADDING_SIZE);
    if (!buf) {
        av_freep(&sc->sample_sizes);
        return AVERROR(ENOMEM);
    }

    if (get_buffer(pb, buf, num_bytes) < num_bytes) {
        av_freep(&sc->sample_sizes);
        av_free(buf);
        return -1;
    }

    init_get_bits(&gb, buf, 8*num_bytes);

    for(i=0; i<entries; i++)
        sc->sample_sizes[i] = get_bits_long(&gb, field_size);

    av_free(buf);
    return 0;
}
