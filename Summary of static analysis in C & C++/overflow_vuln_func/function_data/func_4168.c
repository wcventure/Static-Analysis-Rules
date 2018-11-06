static void read_table(AVFormatContext *avctx, AVStream *st,
                       int (*parse)(AVFormatContext *avctx, AVStream *st,
                                    const char *name, int size))
{
    int count, i;
    AVIOContext *pb = avctx->pb;
    avio_skip(pb, 4);
    count = avio_rb32(pb);
    avio_skip(pb, 4);
    for (i = 0; i < count; i++) {
        char name[17];
        int size;
        avio_read(pb, name, 16);
        name[sizeof(name) - 1] = 0;
        size = avio_rb32(pb);
        if (parse(avctx, st, name, size) < 0) {
            avpriv_request_sample(avctx, "Variable %s", name);
            avio_skip(pb, size);
        }
    }
}
