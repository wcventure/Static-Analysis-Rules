uint8_t *av_packet_pack_dictionary(AVDictionary *dict, int *size)
{
    AVDictionaryEntry *t = NULL;
    uint8_t *data = NULL;
    *size = 0;

    if (!dict)
        return NULL;

    while ((t = av_dict_get(dict, "", t, AV_DICT_IGNORE_SUFFIX))) {
        const int keylen = strlen(t->key);
        const int valuelen = strlen(t->value);
        const size_t new_size = *size + keylen + 1 + valuelen + 1;
        uint8_t *const new_data = av_realloc(data, new_size);

        if (!new_data)
            goto fail;
        data = new_data;

        memcpy(data + *size, t->key, keylen + 1);
        memcpy(data + *size + keylen + 1, t->value, valuelen + 1);

        *size = new_size;
    }

    return data;

fail:
    av_freep(&data);
    *size = 0;
    return NULL;
}
