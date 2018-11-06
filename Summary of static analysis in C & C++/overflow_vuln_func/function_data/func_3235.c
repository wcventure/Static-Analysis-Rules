static void json_print_chapter_header(WriterContext *wctx, const char *chapter)
{
    JSONContext *json = wctx->priv;
    char *chapter_esc;

    if (wctx->nb_chapter)
        printf(",");
    json->multiple_entries = !strcmp(chapter, "packets") || !strcmp(chapter, "streams");
    chapter_esc = json_escape_str(chapter);
    printf("\n  \"%s\":%s", chapter_esc ? chapter_esc : "",
           json->multiple_entries ? " [" : " ");
    av_free(chapter_esc);
}
