static int
dissect_ber_sq_of(gboolean implicit_tag, gint32 type, asn1_ctx_t *actx, proto_tree *parent_tree, tvbuff_t *tvb, int offset, gint32 min_len, gint32 max_len, const ber_sequence_t *seq, gint hf_id, gint ett_id) {
    gint8              classx;
    gboolean           pcx, ind = FALSE, ind_field;
    gint32             tagx;
    guint32            lenx;

    proto_tree        *tree     = parent_tree;
    proto_item        *item     = NULL;
    proto_item        *causex;
    int                cnt, hoffsetx, end_offset;
    header_field_info *hfi;
    gint               length_remaining;
    tvbuff_t          *next_tvb;

#ifdef DEBUG_BER_SQ_OF
{
const char *name;
header_field_info *hfinfo;
if (hf_id >= 0) {
hfinfo = proto_registrar_get_nth(hf_id);
name = hfinfo->name;
} else {
name = "unnamed";
}
if (tvb_length_remaining(tvb,offset) > 3) {
printf("SQ OF dissect_ber_sq_of(%s) entered implicit_tag:%d offset:%d len:%d %02x:%02x:%02x\n", name, implicit_tag, offset, tvb_length_remaining(tvb, offset), tvb_get_guint8(tvb, offset), tvb_get_guint8(tvb, offset+1), tvb_get_guint8(tvb, offset+2));
} else {
printf("SQ OF dissect_ber_sq_of(%s) entered\n", name);
}
}
#endif

    if (!implicit_tag) {
        hoffsetx = offset;
        /
        offset = dissect_ber_identifier(actx->pinfo, tree, tvb, offset, &classx, &pcx, &tagx);
        offset = dissect_ber_length(actx->pinfo, tree, tvb, offset, &lenx, &ind);
        if (ind) {
            /
            end_offset = offset + lenx;
        } else {
            end_offset = offset + lenx;
        }

        /
        if ((classx != BER_CLASS_APP) && (classx != BER_CLASS_PRI)) {
            if (!pcx
             || (!implicit_tag && ((classx != BER_CLASS_UNI)
                                || (tagx != type)))) {
                tvb_ensure_bytes_exist(tvb, hoffsetx, 2);
                causex = proto_tree_add_string_format_value(
                    tree, hf_ber_error, tvb, offset, lenx,
                    (type == BER_UNI_TAG_SEQUENCE) ? "set_of_expected" : "sequence_of_expected",
                    "%s Of expected but class:%s(%d) %s tag:%d was unexpected",
                    (type == BER_UNI_TAG_SEQUENCE) ? "Set" : "Sequence",
                    val_to_str_const(classx, ber_class_codes, "Unknown"),
                    classx, pcx ? ber_pc_codes_short.true_string : ber_pc_codes_short.false_string,
                    tagx);
                if (type == BER_UNI_TAG_SEQUENCE) {
                    expert_add_info_format(
                        actx->pinfo, causex, &ei_ber_expected_set,
                        "BER Error: Set Of expected");
                } else {
                    expert_add_info_format(
                        actx->pinfo, causex, &ei_ber_expected_sequence,
                        "BER Error: Sequence Of expected");
                }
                if (decode_unexpected) {
                    proto_tree *unknown_tree = proto_item_add_subtree(causex, ett_ber_unknown);
                    dissect_unknown_ber(actx->pinfo, tvb, hoffsetx, unknown_tree);
                }
                return end_offset;
            }
        }
    } else {
        /
        lenx = tvb_length_remaining(tvb, offset);
        end_offset = offset + lenx;
    }

    /
    cnt = 0;
    hoffsetx = offset;
    /
    /
    if (tvb_length_remaining(tvb, offset) == tvb_reported_length_remaining(tvb, offset)) {
        while (offset < end_offset) {
            guint32 len;
            gint    s_offset;

            s_offset = offset;

            /
                if ((tvb_get_guint8(tvb, offset) == 0) && (tvb_get_guint8(tvb, offset+1) == 0)) {
                    break;
                }
            /

            /
            offset = get_ber_identifier(tvb, offset, NULL, NULL, NULL);
            offset = get_ber_length(tvb, offset, &len, &ind);
            /
            /
            offset += len;
            cnt++;
            if (offset <= s_offset)
                THROW(ReportedBoundsError);
        }
    }
    offset = hoffsetx;

    /
    if (hf_id >= 0) {
        hfi = proto_registrar_get_nth(hf_id);
        if (parent_tree) {
            if (hfi->type == FT_NONE) {
                item = proto_tree_add_item(parent_tree, hf_id, tvb, offset, lenx, ENC_BIG_ENDIAN);
                proto_item_append_text(item, ":");
            } else {
                item = proto_tree_add_uint(parent_tree, hf_id, tvb, offset, lenx, cnt);
                proto_item_append_text(item, (cnt == 1) ? " item" : " items");
            }
            tree = proto_item_add_subtree(item, ett_id);
            ber_check_items (cnt, min_len, max_len, actx, item);
        }
    }

    /
    while (offset < end_offset) {
        gint8       ber_class;
        gboolean    pc;
        gint32      tag;
        guint32     len;
        int         eoffset;
        int         hoffset;
        proto_item *cause;
        gboolean    imp_tag;

        hoffset = offset;
        /
            if ((tvb_get_guint8(tvb, offset) == 0) && (tvb_get_guint8(tvb, offset+1) == 0)) {
                if (show_internal_ber_fields) {
                    proto_tree_add_text(tree, tvb, hoffset, end_offset-hoffset, "SEQ OF EOC");
                }
                return offset+2;
            }
        /
        /
        offset  = get_ber_identifier(tvb, offset, &ber_class, &pc, &tag);
        offset  = get_ber_length(tvb, offset, &len, &ind_field);
        eoffset = offset + len;
                /
        if (eoffset <= hoffset)
            THROW(ReportedBoundsError);

        if ((ber_class == BER_CLASS_UNI) && (tag == BER_UNI_TAG_EOC)) {
            /
            hoffset = dissect_ber_identifier(actx->pinfo, tree, tvb, hoffset, NULL, NULL, NULL);
            dissect_ber_length(actx->pinfo, tree, tvb, hoffset, NULL, NULL);
            return eoffset;
        }
        /
        /
        if (seq->ber_class != BER_CLASS_ANY) {
          if ((seq->ber_class != ber_class)
           || (seq->tag != tag) ) {
            if (!(seq->flags & BER_FLAGS_NOTCHKTAG)) {
                cause = proto_tree_add_string_format_value(
                    tree, hf_ber_error, tvb, offset, len, "wrong_field",
                    "Wrong field in SQ OF(tag %u expected %u)",
                    tag,
                    seq->tag);
                expert_add_info_format(
                    actx->pinfo, cause, &ei_ber_sequence_field_wrong,
                    "BER Error: Wrong field in Sequence Of");
                if (decode_unexpected) {
                    proto_tree *unknown_tree = proto_item_add_subtree(cause, ett_ber_unknown);
                    dissect_unknown_ber(actx->pinfo, tvb, hoffset, unknown_tree);
                }
                offset = eoffset;
                continue;
                /
            }
          }
        }

        if (!(seq->flags & BER_FLAGS_NOOWNTAG) && !(seq->flags & BER_FLAGS_IMPLTAG)) {
            /
            hoffset = dissect_ber_identifier(actx->pinfo, tree, tvb, hoffset, NULL, NULL, NULL);
            hoffset = dissect_ber_length(actx->pinfo, tree, tvb, hoffset, NULL, NULL);
        }
        if ((seq->flags == BER_FLAGS_IMPLTAG) && (seq->ber_class == BER_CLASS_CON)) {
            /
            /
            hoffset = dissect_ber_identifier(actx->pinfo, tree, tvb, hoffset, NULL, NULL, NULL);
            hoffset = dissect_ber_length(actx->pinfo, tree, tvb, hoffset, NULL, NULL);
            /
        }

        length_remaining = tvb_length_remaining(tvb, hoffset);
        if (length_remaining>eoffset-hoffset)
            length_remaining = eoffset-hoffset;
        next_tvb = tvb_new_subset_length(tvb, hoffset, length_remaining);

        imp_tag = FALSE;
        if (seq->flags == BER_FLAGS_IMPLTAG)
            imp_tag = TRUE;
        /
        seq->func(imp_tag, next_tvb, 0, actx, tree, *seq->p_id);
        /
        cnt++; /
        offset = eoffset;
    }

    /
    if (offset != end_offset) {
        tvb_ensure_bytes_exist(tvb, offset-2, 2);
        causex = proto_tree_add_string_format_value(
            tree, hf_ber_error, tvb, offset-2, 2, "illegal_length",
            "%s Of ate %d too many bytes",
            (type == BER_UNI_TAG_SEQUENCE) ? "Set" : "Sequence",
            offset - end_offset);
        expert_add_info_format(actx->pinfo, causex, &ei_ber_error_length,
            "BER Error:too many byte in %s",
            (type == BER_UNI_TAG_SEQUENCE) ? "Set" : "Sequence");
    }

    return end_offset;
}
