int
dissect_ber_sequence(gboolean implicit_tag, asn1_ctx_t *actx, proto_tree *parent_tree, tvbuff_t *tvb, int offset, const ber_sequence_t *seq, gint hf_id, gint ett_id) {
    gint8       classx;
    gboolean    pcx, ind   = 0, ind_field, imp_tag = FALSE;
    gint32      tagx;
    guint32     lenx;
    proto_tree *tree       = parent_tree;
    proto_item *item       = NULL;
    proto_item *cause;
    int         end_offset = 0;
    int         hoffset;
    gint        length_remaining;
    tvbuff_t   *next_tvb;

#ifdef DEBUG_BER
{
const char *name;
header_field_info *hfinfo;
if (hf_id >= 0) {
hfinfo = proto_registrar_get_nth(hf_id);
name = hfinfo->name;
} else {
name = "unnamed";
}
if (tvb_length_remaining(tvb, offset) > 3) {
printf("SEQUENCE dissect_ber_sequence(%s) entered offset:%d len:%d %02x:%02x:%02x\n", name, offset, tvb_length_remaining(tvb, offset), tvb_get_guint8(tvb, offset), tvb_get_guint8(tvb, offset+1), tvb_get_guint8(tvb, offset+2));
} else {
printf("SEQUENCE dissect_ber_sequence(%s) entered\n", name);
}
}
#endif
    hoffset = offset;
    if (!implicit_tag) {
        offset = get_ber_identifier(tvb, offset, NULL, NULL, NULL);
        offset = get_ber_length(tvb, offset, &lenx, NULL);
    } else {
        /
        lenx = tvb_length_remaining(tvb, offset);
        end_offset = offset+lenx;
    }
    /
    if (hf_id >= 0) {
        if (parent_tree) {
            item = proto_tree_add_item(parent_tree, hf_id, tvb, hoffset, lenx + offset - hoffset, ENC_BIG_ENDIAN);
            tree = proto_item_add_subtree(item, ett_id);
        }
    }
    offset = hoffset;

    if (!implicit_tag) {
        /
        offset = dissect_ber_identifier(actx->pinfo, tree, tvb, offset, &classx, &pcx, &tagx);
        offset = dissect_ber_length(actx->pinfo, tree, tvb, offset, &lenx, &ind);
        if (ind) {
        /
          end_offset = offset + lenx -2;
        } else {
          end_offset = offset + lenx;
        }

        /
        if ((classx != BER_CLASS_APP) && (classx != BER_CLASS_PRI)) {
            if (!pcx
             || (!implicit_tag && ((classx != BER_CLASS_UNI) || (tagx != BER_UNI_TAG_SEQUENCE)))) {
                tvb_ensure_bytes_exist(tvb, hoffset, 2);
                cause = proto_tree_add_string_format_value(
                    tree, hf_ber_error, tvb, offset, lenx, "sequence_expected",
                    "Sequence expected but class:%s(%d) %s tag:%d was unexpected",
                    val_to_str_const(classx, ber_class_codes, "Unknown"),
                    classx,
                    pcx ? ber_pc_codes_short.true_string : ber_pc_codes_short.false_string,
                    tagx);
                expert_add_info(actx->pinfo, cause, &ei_ber_expected_sequence);
                if (decode_unexpected) {
                    proto_tree *unknown_tree = proto_item_add_subtree(cause, ett_ber_unknown);
                    dissect_unknown_ber(actx->pinfo, tvb, hoffset, unknown_tree);
                }
                return end_offset;
            }
        }
    }
    if(offset == end_offset){
        proto_item_append_text(item, " [0 length]");
    }
    /
    while (offset < end_offset) {
        gint8    ber_class;
        gboolean pc;
        gint32   tag;
        guint32  len;
        int      eoffset, count;

        /
            if ((tvb_get_guint8(tvb, offset) == 0) && (tvb_get_guint8(tvb, offset+1) == 0)) {
                /
                offset = dissect_ber_identifier(actx->pinfo, tree, tvb, offset, &ber_class, &pc, &tag);
                dissect_ber_length(actx->pinfo, tree, tvb, offset, &len, &ind);
                proto_item_append_text(item, " 0 items");
                return end_offset;
                /
            }
        /
        hoffset = offset;
        /
        offset = get_ber_identifier(tvb, offset, &ber_class, &pc, &tag);
        offset = get_ber_length(tvb, offset, &len, &ind_field);
        eoffset = offset + len;
                /
        if (eoffset <= hoffset)
            THROW(ReportedBoundsError);

        /

ber_sequence_try_again:
        /
        if (!seq->func) {
            /
            offset = dissect_ber_identifier(actx->pinfo, tree, tvb, hoffset, NULL, NULL, NULL);
            offset = dissect_ber_length(actx->pinfo, tree, tvb, offset, NULL, NULL);
            cause = proto_tree_add_string_format_value(
                tree, hf_ber_error, tvb, offset, len, "unknown_field",
                "This field lies beyond the end of the known sequence definition.");
            expert_add_info(actx->pinfo, cause, &ei_ber_unknown_field_sequence);
            if (decode_unexpected) {
                proto_tree *unknown_tree = proto_item_add_subtree(cause, ett_ber_unknown);
                dissect_unknown_ber(actx->pinfo, tvb, hoffset, unknown_tree);
            }
            offset = eoffset;
            continue;
        }

        /
/
        if ( ((seq->ber_class == BER_CLASS_CON) || (seq->ber_class == BER_CLASS_APP) || (seq->ber_class == BER_CLASS_PRI))
          && (!(seq->flags & BER_FLAGS_NOOWNTAG)) ) {
            if ( (seq->ber_class != BER_CLASS_ANY)
             &&  (seq->tag != -1)
             &&  ( (seq->ber_class != ber_class)
                || (seq->tag != tag) ) ) {
                /
                if (seq->flags & BER_FLAGS_OPTIONAL) {
                    /
                    seq++;
                    goto ber_sequence_try_again;
                }
                offset = dissect_ber_identifier(actx->pinfo, tree, tvb, hoffset, NULL, NULL, NULL);
                offset = dissect_ber_length(actx->pinfo, tree, tvb, offset, NULL, NULL);
                if (seq->ber_class == BER_CLASS_UNI) {
                    cause = proto_tree_add_string_format_value(
                        tree, hf_ber_error, tvb, offset, len, "wrong_field",
                        "Wrong field in SEQUENCE  expected class:%s(%d) tag:%d (%s) but found class:%s(%d) tag:%d",
                        val_to_str_const(seq->ber_class, ber_class_codes, "Unknown"),
                        seq->ber_class,
                        seq->tag,
                        val_to_str_ext_const(seq->tag, &ber_uni_tag_codes_ext, "Unknown"),
                        val_to_str_const(ber_class, ber_class_codes, "Unknown"),
                        ber_class,
                        tag);
                    expert_add_info(actx->pinfo, cause, &ei_ber_sequence_field_wrong);
                } else {
                    cause = proto_tree_add_string_format_value(
                        tree, hf_ber_error, tvb, offset, len, "wrong_field",
                        "Wrong field in SEQUENCE  expected class:%s(%d) tag:%d but found class:%s(%d) tag:%d",
                        val_to_str_const(seq->ber_class, ber_class_codes, "Unknown"),
                        seq->ber_class,
                        seq->tag,
                        val_to_str_const(ber_class, ber_class_codes, "Unknown"),
                        ber_class,
                        tag);
                    expert_add_info(actx->pinfo, cause, &ei_ber_sequence_field_wrong);
                }
                if (decode_unexpected) {
                    proto_tree *unknown_tree = proto_item_add_subtree(cause, ett_ber_unknown);
                    dissect_unknown_ber(actx->pinfo, tvb, hoffset, unknown_tree);
                }
                seq++;
                offset = eoffset;
                continue;
            }
        } else if (!(seq->flags & BER_FLAGS_NOTCHKTAG)) {
            if ( (seq->ber_class != BER_CLASS_ANY)
              && (seq->tag != -1)
              && ( (seq->ber_class != ber_class)
                || (seq->tag != tag) ) ) {
                /
                if (seq->flags & BER_FLAGS_OPTIONAL) {
                    /
                    seq++;
                    goto ber_sequence_try_again;
                }

                offset = dissect_ber_identifier(actx->pinfo, tree, tvb, hoffset, NULL, NULL, NULL);
                offset = dissect_ber_length(actx->pinfo, tree, tvb, offset, NULL, NULL);
                if ( seq->ber_class == BER_CLASS_UNI) {
                    cause = proto_tree_add_string_format_value(
                        tree, hf_ber_error, tvb, offset, len, "wrong_field",
                        "Wrong field in sequence  expected class:%s(%d) tag:%d(%s) but found class:%s(%d) tag:%d",
                        val_to_str_const(seq->ber_class, ber_class_codes, "Unknown"),
                        seq->ber_class,
                        seq->tag,
                        val_to_str_ext_const(seq->tag, &ber_uni_tag_codes_ext, "Unknown"),
                        val_to_str_const(ber_class, ber_class_codes, "Unknown"),
                        ber_class, tag);
                    expert_add_info(actx->pinfo, cause, &ei_ber_sequence_field_wrong);
                } else {
                    cause = proto_tree_add_string_format_value(
                        tree, hf_ber_error, tvb, offset, len, "wrong_field",
                        "Wrong field in sequence  expected class:%s(%d) tag:%d but found class:%s(%d) tag:%d",
                        val_to_str_const(seq->ber_class, ber_class_codes, "Unknown"),
                        seq->ber_class,
                        seq->tag,
                        val_to_str_const(ber_class, ber_class_codes, "Unknown"),
                        ber_class,
                        tag);
                    expert_add_info(actx->pinfo, cause, &ei_ber_sequence_field_wrong);
                }
                if (decode_unexpected) {
                    proto_tree *unknown_tree = proto_item_add_subtree(cause, ett_ber_unknown);
                    dissect_unknown_ber(actx->pinfo, tvb, hoffset, unknown_tree);
                }
                seq++;
                offset = eoffset;
                continue;
            }
        }

        if (!(seq->flags & BER_FLAGS_NOOWNTAG) ) {
            /
            if (ind_field && (len == 2)) {
                /
                next_tvb = tvb_new_subset_length(tvb, offset, len);
                hoffset = eoffset;
            } else {
                hoffset = dissect_ber_identifier(actx->pinfo, tree, tvb, hoffset, NULL, NULL, NULL);
                hoffset = dissect_ber_length(actx->pinfo, tree, tvb, hoffset, NULL, NULL);
                length_remaining = tvb_length_remaining(tvb, hoffset);
                if (length_remaining > (eoffset - hoffset - (2 * ind_field)))
                    length_remaining = eoffset - hoffset - (2 * ind_field);
                next_tvb = tvb_new_subset_length(tvb, hoffset, length_remaining);
            }
        } else {
            length_remaining = tvb_length_remaining(tvb, hoffset);
            if (length_remaining > (eoffset - hoffset))
                length_remaining = eoffset - hoffset;
            next_tvb = tvb_new_subset_length(tvb, hoffset, length_remaining);
        }

#if 0
        /
        if ((eoffset-hoffset) > length_remaining) {
        /         * length) of if the tvb is short, then just
         * give it all of the tvb and hope for the best.
         */
            next_tvb = tvb_new_subset_remaining(tvb, hoffset);
        } else {

        }
#endif

#ifdef DEBUG_BER
{
const char *name;
header_field_info *hfinfo;
if (hf_id >= 0) {
hfinfo = proto_registrar_get_nth(hf_id);
name = hfinfo->name;
} else {
name = "unnamed";
}
if (tvb_length_remaining(next_tvb, 0) > 3) {
printf("SEQUENCE dissect_ber_sequence(%s) calling subdissector offset:%d len:%d %02x:%02x:%02x\n", name, offset, tvb_length_remaining(next_tvb, 0), tvb_get_guint8(next_tvb, 0), tvb_get_guint8(next_tvb, 1), tvb_get_guint8(next_tvb, 2));
} else {
printf("SEQUENCE dissect_ber_sequence(%s) calling subdissector\n", name);
}
}
#endif
        if (next_tvb == NULL) {
            /
            THROW(ReportedBoundsError);
        }
        imp_tag = FALSE;
        if (seq->flags & BER_FLAGS_IMPLTAG) {
            imp_tag = TRUE;
        }

        count = seq->func(imp_tag, next_tvb, 0, actx, tree, *seq->p_id);

#ifdef DEBUG_BER
{
const char *name;
header_field_info *hfinfo;
if (hf_id >= 0) {
hfinfo = proto_registrar_get_nth(hf_id);
name = hfinfo->name;
} else {
name = "unnamed";
}
printf("SEQUENCE dissect_ber_sequence(%s) subdissector ate %d bytes\n", name, count);
}
#endif
        /
        /
        if ((len != 0) && (count == 0) && (seq->flags & BER_FLAGS_OPTIONAL)) {
            seq++;
            goto ber_sequence_try_again;
        /
        }
        offset = eoffset;
        if (!(seq->flags & BER_FLAGS_NOOWNTAG) ) {
            /
            if ((ind_field == 1) && (len > 2))
            {
                /
                if (show_internal_ber_fields) {
                    proto_tree_add_text(tree, tvb, offset, count, "SEQ FIELD EOC");
                }
            }
        }
        seq++;
    }

    /
    if (offset != end_offset) {
        tvb_ensure_bytes_exist(tvb, offset-2, 2);
        cause = proto_tree_add_string_format_value(
            tree, hf_ber_error, tvb, offset-2, 2, "illegal_length",
            "Sequence ate %d too many bytes",
            offset - end_offset);
        expert_add_info_format(actx->pinfo, cause, &ei_ber_error_length,
            "BER Error: too many bytes in Sequence");
    }
    if (ind) {
        /
        end_offset += 2;
        if (show_internal_ber_fields) {
            proto_tree_add_text(tree, tvb, end_offset-2, 2 , "SEQ EOC");
        }
    }
    return end_offset;
}
