static inline void json_print_item_str(WriterContext *wctx,
                                       const char *key, const char *value,
                                       const char *indent)
{
    char *key_esc = json_escape_str(key);
    char *value_esc = json_escape_str(value);

    printf("%s\"%s\": \"%s\"", indent,
           key_esc   ? key_esc   : "",
           value_esc ? value_esc : "");
    av_free(key_esc);
    av_free(value_esc);
}
