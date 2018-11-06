int
dissect_ber_tagged_type(gboolean implicit_tag, asn1_ctx_t *actx, proto_tree *tree, tvbuff_t *tvb, int offset, gint hf_id, gint8 tag_cls, gint32 tag_tag, gboolean tag_impl, ber_type_fn type)
{
    gint8       tmp_cls;
    gint32      tmp_tag;
    guint32     tmp_len;
    tvbuff_t   *next_tvb = tvb;
    proto_item *cause;

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
printf("dissect_ber_tagged_type(%s) entered implicit_tag:%d offset:%d len:%d %02x:%02x:%02x\n", name, implicit_tag, offset, tvb_length_remaining(tvb, offset), tvb_get_guint8(tvb, offset), tvb_get_guint8(tvb, offset+1), tvb_get_guint8(tvb, offset+2));
} else {
printf("dissect_ber_tagged_type(%s) entered\n", name);
}
}
#endif

    if (implicit_tag) {
        offset = type(tag_impl, tvb, offset, actx, tree, hf_id);
        return offset;
    }

    offset = dissect_ber_identifier(actx->pinfo, tree, tvb, offset, &tmp_cls, NULL, &tmp_tag);
    offset = dissect_ber_length(actx->pinfo, tree, tvb, offset, &tmp_len, NULL);

    if ((tmp_cls != tag_cls) || (tmp_tag != tag_tag)) {
        cause = proto_tree_add_string_format_value(
            tree, hf_ber_error, tvb, offset, tmp_len, "wrong_tag",
            "Wrong tag in tagged type - expected class:%s(%d) tag:%d (%s) but found class:%s(%d) tag:%d",
            val_to_str_const(tag_cls, ber_class_codes, "Unknown"),
            tag_cls,
            tag_tag,
            val_to_str_ext_const(tag_tag, &ber_uni_tag_codes_ext, "Unknown"),
            val_to_str_const(tmp_cls, ber_class_codes, "Unknown"),
            tmp_cls,
            tmp_tag);
        expert_add_info(actx->pinfo, cause, &ei_ber_wrong_tag_in_tagged_type);
    }

    if (tag_impl) {
        gint length_remaining;

        length_remaining = tvb_length_remaining(tvb, offset);
        if (tmp_len > (guint32)length_remaining)
            tmp_len = length_remaining;
        next_tvb = tvb_new_subset_length(tvb, offset, tmp_len);
        type(tag_impl, next_tvb, 0, actx, tree, hf_id);
        offset += tmp_len;
    } else {
        offset = type(tag_impl, tvb, offset, actx, tree, hf_id);
    }

    return offset;
}
