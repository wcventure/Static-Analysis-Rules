static int
reassemble_octet_string(asn1_ctx_t *actx, proto_tree *tree, gint hf_id, tvbuff_t *tvb, int offset, guint32 con_len, gboolean ind, tvbuff_t **out_tvb)
{
    fragment_head *fd_head         = NULL;
    tvbuff_t      *next_tvb        = NULL;
    tvbuff_t      *reassembled_tvb = NULL;
    guint16        dst_ref         = 0;
    int            start_offset    = offset;
    gboolean       fragment        = TRUE;
    gboolean       firstFragment   = TRUE;

    /

    if (out_tvb)
        *out_tvb = NULL;

    if (con_len == 0) /
        return offset;

    /
    actx->pinfo->fragmented = TRUE;

    while(!fd_head) {

        offset = dissect_ber_octet_string(FALSE, actx, NULL, tvb, offset, hf_id, &next_tvb);

        if (next_tvb == NULL) {
            /
            THROW(ReportedBoundsError);
        }

        if (ind) {
            /

            if ((tvb_get_guint8(tvb, offset) == 0) && (tvb_get_guint8(tvb, offset+1) == 0)) {
                fragment = FALSE;
                /
                offset +=2;
            }
        } else {

            if ((guint32)(offset - start_offset) >= con_len)
                fragment = FALSE;
        }

        if (!fragment && firstFragment) {
            /
            /
            gboolean pc;
            get_ber_identifier(tvb, start_offset, NULL, &pc, NULL);
            if (!pc && tree) {
                /
                dissect_ber_octet_string(FALSE, actx, tree, tvb, start_offset, hf_id, NULL);
            }
            reassembled_tvb = next_tvb;
            break;
        }


        if (tvb_reported_length(next_tvb) < 1) {
            /
            THROW(ReportedBoundsError);
        }
        fd_head = fragment_add_seq_next(&octet_segment_reassembly_table,
                                        next_tvb, 0, actx->pinfo, dst_ref, NULL,
                                        tvb_reported_length(next_tvb),
                                        fragment);

        firstFragment = FALSE;
    }

    if (fd_head) {
        if (fd_head->next) {
            /
            proto_tree *next_tree;
            proto_item *frag_tree_item;

            reassembled_tvb = tvb_new_chain(next_tvb, fd_head->tvb_data);

            actx->created_item = proto_tree_add_item(tree, hf_id, reassembled_tvb, 0, -1, ENC_BIG_ENDIAN);
            next_tree = proto_item_add_subtree (actx->created_item, ett_ber_reassembled_octet_string);

            add_new_data_source(actx->pinfo, reassembled_tvb, "Reassembled OCTET STRING");
            show_fragment_seq_tree(fd_head, &octet_string_frag_items, next_tree, actx->pinfo, reassembled_tvb, &frag_tree_item);
        }
    }

    if (out_tvb)
        *out_tvb = reassembled_tvb;

    /
    actx->pinfo->fragmented = FALSE;

    return offset;

}
