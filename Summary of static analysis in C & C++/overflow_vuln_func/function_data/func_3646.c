static int open_input(HLSContext *c, struct playlist *pls, struct segment *seg)
{
    AVDictionary *opts = NULL;
    int ret;

    // broker prior HTTP options that should be consistent across requests
    av_dict_set(&opts, "user-agent", c->user_agent, 0);
    av_dict_set(&opts, "cookies", c->cookies, 0);
    av_dict_set(&opts, "headers", c->headers, 0);
    av_dict_set(&opts, "http_proxy", c->http_proxy, 0);
    av_dict_set(&opts, "seekable", "0", 0);

    if (seg->size >= 0) {
        /
        av_dict_set_int(&opts, "offset", seg->url_offset, 0);
        av_dict_set_int(&opts, "end_offset", seg->url_offset + seg->size, 0);
    }

    av_log(pls->parent, AV_LOG_VERBOSE, "HLS request for url '%s', offset %"PRId64", playlist %d\n",
           seg->url, seg->url_offset, pls->index);

    if (seg->key_type == KEY_NONE) {
        ret = open_url(pls->parent, &pls->input, seg->url, c->avio_opts, opts);
    } else if (seg->key_type == KEY_AES_128) {
        AVDictionary *opts2 = NULL;
        char iv[33], key[33], url[MAX_URL_SIZE];
        if (strcmp(seg->key, pls->key_url)) {
            AVIOContext *pb;
            if (open_url(pls->parent, &pb, seg->key, c->avio_opts, opts) == 0) {
                ret = avio_read(pb, pls->key, sizeof(pls->key));
                if (ret != sizeof(pls->key)) {
                    av_log(NULL, AV_LOG_ERROR, "Unable to read key file %s\n",
                           seg->key);
                }
                ff_format_io_close(pls->parent, &pb);
            } else {
                av_log(NULL, AV_LOG_ERROR, "Unable to open key file %s\n",
                       seg->key);
            }
            av_strlcpy(pls->key_url, seg->key, sizeof(pls->key_url));
        }
        ff_data_to_hex(iv, seg->iv, sizeof(seg->iv), 0);
        ff_data_to_hex(key, pls->key, sizeof(pls->key), 0);
        iv[32] = key[32] = '\0';
        if (strstr(seg->url, "://"))
            snprintf(url, sizeof(url), "crypto+%s", seg->url);
        else
            snprintf(url, sizeof(url), "crypto:%s", seg->url);

        av_dict_copy(&opts2, c->avio_opts, 0);
        av_dict_set(&opts2, "key", key, 0);
        av_dict_set(&opts2, "iv", iv, 0);

        ret = open_url(pls->parent, &pls->input, url, opts2, opts);

        av_dict_free(&opts2);

        if (ret < 0) {
            goto cleanup;
        }
        ret = 0;
    } else if (seg->key_type == KEY_SAMPLE_AES) {
        av_log(pls->parent, AV_LOG_ERROR,
               "SAMPLE-AES encryption is not supported yet\n");
        ret = AVERROR_PATCHWELCOME;
    }
    else
      ret = AVERROR(ENOSYS);

    /
    if (ret == 0 && seg->key_type == KEY_NONE && seg->url_offset) {
        int seekret = avio_seek(pls->input, seg->url_offset, SEEK_SET);
        if (seekret < 0) {
            av_log(pls->parent, AV_LOG_ERROR, "Unable to seek to offset %"PRId64" of HLS segment '%s'\n", seg->url_offset, seg->url);
            ret = seekret;
            ff_format_io_close(pls->parent, &pls->input);
        }
    }

cleanup:
    av_dict_free(&opts);
    pls->cur_seg_offset = 0;
    return ret;
}
