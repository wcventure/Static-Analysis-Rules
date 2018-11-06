static int mkv_write_chapters(AVFormatContext *s)
{
    MatroskaMuxContext *mkv = s->priv_data;
    AVIOContext *pb = s->pb;
    ebml_master chapters, editionentry;
    AVRational scale = {1, 1E9};
    int i, ret;

    if (!s->nb_chapters || mkv->wrote_chapters)
        return 0;

    ret = mkv_add_seekhead_entry(mkv->main_seekhead, MATROSKA_ID_CHAPTERS, avio_tell(pb));
    if (ret < 0) return ret;

    chapters     = start_ebml_master(pb, MATROSKA_ID_CHAPTERS    , 0);
    editionentry = start_ebml_master(pb, MATROSKA_ID_EDITIONENTRY, 0);
    put_ebml_uint(pb, MATROSKA_ID_EDITIONFLAGDEFAULT, 1);
    put_ebml_uint(pb, MATROSKA_ID_EDITIONFLAGHIDDEN , 0);
    for (i = 0; i < s->nb_chapters; i++) {
        ebml_master chapteratom, chapterdisplay;
        AVChapter *c     = s->chapters[i];
        int chapterstart = av_rescale_q(c->start, c->time_base, scale);
        int chapterend   = av_rescale_q(c->end,   c->time_base, scale);
        AVDictionaryEntry *t = NULL;
        if (chapterstart < 0 || chapterstart > chapterend)
            return AVERROR_INVALIDDATA;

        chapteratom = start_ebml_master(pb, MATROSKA_ID_CHAPTERATOM, 0);
        put_ebml_uint(pb, MATROSKA_ID_CHAPTERUID, c->id + mkv->chapter_id_offset);
        put_ebml_uint(pb, MATROSKA_ID_CHAPTERTIMESTART, chapterstart);
        put_ebml_uint(pb, MATROSKA_ID_CHAPTERTIMEEND, chapterend);
        put_ebml_uint(pb, MATROSKA_ID_CHAPTERFLAGHIDDEN , 0);
        put_ebml_uint(pb, MATROSKA_ID_CHAPTERFLAGENABLED, 1);
        if ((t = av_dict_get(c->metadata, "title", NULL, 0))) {
            chapterdisplay = start_ebml_master(pb, MATROSKA_ID_CHAPTERDISPLAY, 0);
            put_ebml_string(pb, MATROSKA_ID_CHAPSTRING, t->value);
            put_ebml_string(pb, MATROSKA_ID_CHAPLANG  , "und");
            end_ebml_master(pb, chapterdisplay);
        }
        end_ebml_master(pb, chapteratom);
    }
    end_ebml_master(pb, editionentry);
    end_ebml_master(pb, chapters);

    mkv->wrote_chapters = 1;
    return 0;
}
