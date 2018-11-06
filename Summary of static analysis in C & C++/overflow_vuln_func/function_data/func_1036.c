static int
dissect_ber_any_oid(gboolean implicit_tag, asn1_ctx_t *actx, proto_tree *tree, tvbuff_t *tvb, int offset, gint hf_id, tvbuff_t **value_tvb, gboolean is_absolute)
{
    gint8        ber_class;
    gboolean     pc;
    gint32       tag;
    guint32      len;
    int          eoffset;
    int          hoffset;
    const char  *str;
    proto_item  *cause;
    const gchar *name;
    header_field_info *hfi;

#ifdef DEBUG_BER
{
header_field_info *hfinfo;
if (hf_id >= 0) {
hfinfo = proto_registrar_get_nth(hf_id);
name = hfinfo->name;
} else {
name = "unnamed";
}
if (tvb_length_remaining(tvb, offset) > 3) {
printf("OBJECT IDENTIFIER dissect_ber_any_oid(%s) entered implicit_tag:%d offset:%d len:%d %02x:%02x:%02x\n", name, implicit_tag, offset, tvb_length_remaining(tvb, offset), tvb_get_guint8(tvb, offset), tvb_get_guint8(tvb, offset+1), tvb_get_guint8(tvb, offset+2));
} else {
printf("OBJECT IDENTIFIER dissect_ber_any_oid(%s) entered\n", name);
}
}
#endif

    if (!implicit_tag) {
        hoffset = offset;
        /
        offset = dissect_ber_identifier(actx->pinfo, tree, tvb, offset, &ber_class, &pc, &tag);
        offset = dissect_ber_length(actx->pinfo, tree, tvb, offset, &len, NULL);
        eoffset = offset + len;
        if ( (ber_class != BER_CLASS_UNI)
          || (is_absolute && tag != BER_UNI_TAG_OID)
          || (!is_absolute && tag != BER_UNI_TAG_RELATIVE_OID) ) {
                tvb_ensure_bytes_exist(tvb, hoffset, 2);
            cause = proto_tree_add_string_format_value(
                tree, hf_ber_error, tvb, offset, len, "oid_expected",
                "Object Identifier expected but class:%s(%d) %s tag:%d was unexpected",
                val_to_str_const(ber_class, ber_class_codes, "Unknown"),
                ber_class,
                pc ? ber_pc_codes_short.true_string : ber_pc_codes_short.false_string,
                tag);
            expert_add_info(actx->pinfo, cause, &ei_ber_expected_object_identifier);
            if (decode_unexpected) {
                proto_tree *unknown_tree = proto_item_add_subtree(cause, ett_ber_unknown);
                dissect_unknown_ber(actx->pinfo, tvb, hoffset, unknown_tree);
            }
            return eoffset;
        }
    } else {
        len = tvb_length_remaining(tvb, offset);
        eoffset = offset+len;
    }

    actx->created_item = NULL;
    hfi = proto_registrar_get_nth(hf_id);
    if ((is_absolute && hfi->type == FT_OID) || (!is_absolute && hfi->type == FT_REL_OID)) {
        actx->created_item = proto_tree_add_item(tree, hf_id, tvb, offset, len, ENC_BIG_ENDIAN);
    } else if (IS_FT_STRING(hfi->type)) {
        str = oid_encoded2string(tvb_get_ptr(tvb, offset, len), len);
        actx->created_item = proto_tree_add_string(tree, hf_id, tvb, offset, len, str);
        if (actx->created_item) {
            /
            name = oid_resolved_from_encoded(tvb_get_ptr(tvb, offset, len), len);
            if (name) {
                proto_item_append_text(actx->created_item, " (%s)", name);
            }
        }
    } else {
        DISSECTOR_ASSERT_NOT_REACHED();
    }

    if (value_tvb)
        *value_tvb = tvb_new_subset_length(tvb, offset, len);

    return eoffset;
}
