static void mov_parse_stsd_subtitle(MOVContext *c, AVIOContext *pb,
                                    AVStream *st, MOVStreamContext *sc,
                                    int size)
{
    // ttxt stsd contains display flags, justification, background
    // color, fonts, and default styles, so fake an atom to read it
    MOVAtom fake_atom = { .size = size };
    // mp4s contains a regular esds atom
    if (st->codec->codec_tag != AV_RL32("mp4s"))
        mov_read_glbl(c, pb, fake_atom);
    st->codec->width  = sc->width;
    st->codec->height = sc->height;
}
