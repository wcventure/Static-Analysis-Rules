static int dissect_media( const gchar* fullmediatype, tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree) {
    int dissected = 0;

    if (fullmediatype) {
        gchar *mediatype = wmem_strdup(wmem_packet_scope(), fullmediatype);
        gchar *parms_at = strchr(mediatype, ';');
        const char *save_match_string = pinfo->match_string;
        char *media_str = NULL;

        /
        if (NULL != parms_at) {
            media_str = wmem_strdup( wmem_packet_scope(), parms_at + 1 );
            *parms_at = '\0';
        }

        /
        pinfo->match_string = wmem_strdup(wmem_packet_scope(), mediatype);

        /
        ascii_strdown_inplace(mediatype);

        if (0 == strcmp("application/x-jxta-tls-block", mediatype)) {
            /
            if (NULL != ssl_handle) {
                dissected = call_dissector(ssl_handle, tvb, pinfo, tree);
            }
        } else if (0 == strcmp("application/gzip", mediatype)) {
            tvbuff_t *uncomp_tvb = tvb_child_uncompress(tvb, tvb, 0, tvb_captured_length(tvb));

            if( NULL != uncomp_tvb ) {
                add_new_data_source(pinfo, uncomp_tvb, "Uncompressed Element Content");

                /
                dissected = dissect_media("text/xml;charset=\"UTF-8\"", uncomp_tvb, pinfo, tree);

                if( dissected > 0 ) {
                    /
                    dissected = tvb_captured_length(tvb);
                }
            }
        } else {
            dissected = dissector_try_string(media_type_dissector_table, mediatype, tvb, pinfo, tree, media_str) ? tvb_captured_length(tvb) : 0;

            if( dissected != (int) tvb_captured_length(tvb) ) {
                /
            }
        }

        if (0 == dissected) {
            dissected = call_dissector_with_data(media_handle, tvb, pinfo, tree, media_str);
        }

        pinfo->match_string = save_match_string;
    }

    if(0 == dissected) {
        /
        dissected = call_data_dissector(tvb, pinfo, tree);
    }

    return dissected;
}
