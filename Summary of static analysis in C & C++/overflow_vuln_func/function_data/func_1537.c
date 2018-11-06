static void
add_multipart_data (proto_tree *tree, tvbuff_t *tvb, packet_info *pinfo)
{
    int         offset      = 0;
    guint       nextOffset;
    guint       nEntries    = 0;
    guint       count;
    guint       HeadersLen;
    guint       DataLen;
    guint       contentType = 0;
    const char *contentTypeStr;
    tvbuff_t   *tmp_tvb;
    int         partnr      = 1;
    int         part_start;
    int         found_match = 0;

    proto_item *sub_tree   = NULL;
    proto_item *ti         = NULL;
    proto_tree *mpart_tree = NULL;

    heur_dtbl_entry_t       *hdtbl_entry;

    nEntries = tvb_get_guintvar (tvb, offset, &count);
    offset += count;
    if (nEntries)
    {
        sub_tree = proto_tree_add_subtree(tree, tvb, offset - count, 0,
                    ett_mpartlist, NULL, "Multipart body");
    }
    while (nEntries--)
    {
        part_start = offset;
        HeadersLen = tvb_get_guintvar (tvb, offset, &count);
        offset += count;
        DataLen = tvb_get_guintvar (tvb, offset, &count);
        offset += count;

        ti = proto_tree_add_uint(sub_tree, hf_wsp_mpart, tvb, part_start,
                    HeadersLen + DataLen + (offset - part_start), partnr);
        mpart_tree = proto_item_add_subtree(ti, ett_multiparts);

        nextOffset = add_content_type (mpart_tree, pinfo, tvb, offset,
                &contentType, &contentTypeStr);

        if (tree) {
            /
            if (contentTypeStr) {
                proto_item_append_text(ti, ", content-type: %s",
                        contentTypeStr);
            } else {
                proto_item_append_text(ti, ", content-type: 0x%X",
                        contentType);
            }
        }

        HeadersLen -= (nextOffset - offset);
        if (HeadersLen > 0)
        {
            tmp_tvb = tvb_new_subset_length (tvb, nextOffset, HeadersLen);
            add_headers (mpart_tree, tmp_tvb, hf_wsp_headers_section, pinfo);
        }
        offset = nextOffset + HeadersLen;
        /
        tmp_tvb = tvb_new_subset_length(tvb, offset, DataLen);
        /
        found_match = 0;
        if (contentTypeStr) {
            /
            found_match = dissector_try_string(media_type_table,
                    contentTypeStr, tmp_tvb, pinfo, mpart_tree, NULL);
        }
        if (! found_match) {
            if (! dissector_try_heuristic(heur_subdissector_list,
                        tmp_tvb, pinfo, mpart_tree, &hdtbl_entry, NULL)) {

                pinfo->match_string = contentTypeStr;
                call_dissector_with_data(media_handle, tmp_tvb, pinfo, mpart_tree, NULL /);
#if 0
                if (tree) /
                    proto_tree_add_item (mpart_tree, hf_wsp_multipart_data,
                            tvb, offset, DataLen, ENC_NA);
#endif
            }
        }

        offset += DataLen;
        partnr++;
    }
}
