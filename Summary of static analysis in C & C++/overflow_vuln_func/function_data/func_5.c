int
dissect_ber_constrained_octet_string(gboolean implicit_tag, asn1_ctx_t *actx, proto_tree *tree, tvbuff_t *tvb, int offset, gint32 min_len, gint32 max_len, gint hf_id, tvbuff_t **out_tvb) {
    gint8       ber_class;
    gboolean    pc, ind;
    gint32      tag;
    guint32     len;
    guint       encoding;
    int         hoffset;
    int         end_offset;
    proto_item *it, *cause;
    guint32     len_remain;

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
if (tvb_reported_length_remaining(tvb, offset) > 3) {
printf("OCTET STRING dissect_ber_octet string(%s) entered implicit_tag:%d offset:%d len:%d %02x:%02x:%02x\n", name, implicit_tag, offset, tvb_reported_length_remaining(tvb, offset), tvb_get_guint8(tvb, offset), tvb_get_guint8(tvb, offset+1), tvb_get_guint8(tvb, offset+2));
} else {
printf("OCTET STRING dissect_ber_octet_string(%s) entered\n", name);
}
}
#endif

    if (out_tvb)
        *out_tvb = NULL;

    if (!implicit_tag) {
        hoffset = offset;
        /
        offset = dissect_ber_identifier(actx->pinfo, tree, tvb, offset, &ber_class, &pc, &tag);
        offset = dissect_ber_length(actx->pinfo, tree, tvb, offset, &len, &ind);
        end_offset = offset+len;

        /
        if ((ber_class != BER_CLASS_APP) && (ber_class != BER_CLASS_PRI)) {
            if ( (ber_class != BER_CLASS_UNI)
              || ((tag < BER_UNI_TAG_NumericString) && (tag != BER_UNI_TAG_OCTETSTRING) && (tag != BER_UNI_TAG_UTF8String)) ) {
                tvb_ensure_bytes_exist(tvb, hoffset, 2);
                cause = proto_tree_add_string_format_value(
                    tree, hf_ber_error, tvb, offset, len, "octetstring_expected",
                    "OctetString expected but class:%s(%d) %s tag:%d was unexpected",
                    val_to_str_const(ber_class, ber_class_codes, "Unknown"),
                    ber_class,
                    pc ? ber_pc_codes_short.true_string : ber_pc_codes_short.false_string,
                    tag);
                expert_add_info(actx->pinfo, cause, &ei_ber_expected_octet_string);
                if (decode_unexpected) {
                    proto_tree *unknown_tree = proto_item_add_subtree(cause, ett_ber_unknown);
                    dissect_unknown_ber(actx->pinfo, tvb, hoffset, unknown_tree);
                }
                return end_offset;
            }
        }
    } else {
        /

        get_last_ber_identifier(&ber_class, &pc, &tag);
        get_last_ber_length(&len, &ind);

        end_offset = offset+len;

        /
        len_remain = (guint32)tvb_reported_length_remaining(tvb, offset);
        if (ind && (len_remain == (len - 2))) {
            /
            len -= 2;
            end_offset -= 2;
            ind = FALSE;
        } else if (len_remain < len) {
            /
            cause = proto_tree_add_string_format_value(
                tree, hf_ber_error, tvb, offset, len, "illegal_length",
                "length:%u longer than tvb_reported_length_remaining:%d",
                len,
                len_remain);
            expert_add_info(actx->pinfo, cause, &ei_ber_error_length);
            return end_offset;
        }

    }

    actx->created_item = NULL;

    if (pc) {
        /
        end_offset = reassemble_octet_string(actx, tree, hf_id, tvb, offset, len, ind, out_tvb);
    } else {
        /
        gint length_remaining;

        length_remaining = tvb_reported_length_remaining(tvb, offset);
#if 0
        if (length_remaining < 1) {
            return end_offset;
        }
#endif

        if (len <= (guint32)length_remaining) {
            length_remaining = len;
        }
        if (hf_id >= 0) {
            /
            switch (tag) {

            case BER_UNI_TAG_UTF8String:
                /
                encoding = ENC_UTF_8|ENC_NA;
                break;

            case BER_UNI_TAG_NumericString:
            case BER_UNI_TAG_PrintableString:
            case BER_UNI_TAG_VisibleString:
            case BER_UNI_TAG_IA5String:
                /
                encoding = ENC_ASCII|ENC_NA;
                break;

            case BER_UNI_TAG_TeletexString:
                /
                encoding = ENC_ASCII|ENC_NA;
                break;

            case BER_UNI_TAG_VideotexString:
                /
                encoding = ENC_ASCII|ENC_NA;
                break;

            case BER_UNI_TAG_GraphicString:
            case BER_UNI_TAG_GeneralString:
                /
                encoding = ENC_ASCII|ENC_NA;
                break;

            case BER_UNI_TAG_UniversalString:
                /
                encoding = ENC_UCS_4|ENC_BIG_ENDIAN;
                break;

            case BER_UNI_TAG_CHARACTERSTRING:
                /
                encoding = ENC_ASCII|ENC_NA;
                break;

            case BER_UNI_TAG_BMPString:
                /
                encoding = ENC_UCS_2|ENC_BIG_ENDIAN;
                break;

            default:
                 encoding = ENC_BIG_ENDIAN;
                 break;
            }
            it = ber_proto_tree_add_item(actx->pinfo, tree, hf_id, tvb, offset, length_remaining, encoding);
            actx->created_item = it;
            ber_check_length(length_remaining, min_len, max_len, actx, it, FALSE);
        } else {

            proto_tree_add_item(tree, hf_ber_unknown_octetstring, tvb, offset, len, ENC_NA);
        }

        if (out_tvb) {
            *out_tvb = ber_tvb_new_subset_length(tvb, offset, len);
        }
    }
    return end_offset;
}
