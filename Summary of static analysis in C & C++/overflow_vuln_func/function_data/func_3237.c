static void json_print_int(WriterContext *wctx, const char *key, int value)
{
    char *key_esc = json_escape_str(key);

    if (wctx->nb_item) printf(",\n");
    printf(INDENT "\"%s\": %d", key_esc ? key_esc : "", value);
    av_free(key_esc);
}
