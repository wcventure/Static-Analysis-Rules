static int compare_codec_desc(const void *a, const void *b)
{
    const AVCodecDescriptor * const *da = a;
    const AVCodecDescriptor * const *db = b;

    return (*da)->type != (*db)->type ? (*da)->type - (*db)->type :
           strcmp((*da)->name, (*db)->name);
}
