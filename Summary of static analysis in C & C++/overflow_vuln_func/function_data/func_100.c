static guint
fTagHeaderTree (tvbuff_t *tvb, proto_tree *tree, guint offset,
    guint8 *tag_no, guint8* tag_info, guint32 *lvt)
{
    guint8      tag;
    guint8      value;
    guint       tag_len = 1;
    guint       lvt_len = 1;
    guint       lvt_offset;
    proto_item *ti;
    proto_tree *subtree;

    lvt_offset = offset;
    tag        = tvb_get_guint8(tvb, offset);
    *tag_info  = 0;
    *lvt       = tag & 0x07;

	
    if (tag_is_context_specific(tag)) *tag_info = tag & 0x0F;
    *tag_no = tag >> 4;
    if (tag_is_extended_tag_number(tag)) {
        *tag_no = tvb_get_guint8(tvb, offset + tag_len++);
    }
    if (tag_is_extended_value(tag)) {
        lvt_offset += tag_len;
        value = tvb_get_guint8(tvb, lvt_offset);
        tag_len++;
        if (value == 254) {
            *lvt = tvb_get_ntohs(tvb, lvt_offset+1);
            tag_len += 2;
            lvt_len += 2;
        } else if (value == 255) {
            *lvt = tvb_get_ntohl(tvb, lvt_offset+1);
            tag_len += 4;
            lvt_len += 4;
        } else
            *lvt = value;
    }

    if (tree) {
        if (tag_is_opening(tag))
            ti = proto_tree_add_text(tree, tvb, offset, tag_len, "{[%u]", *tag_no );
        else if (tag_is_closing(tag))
            ti = proto_tree_add_text(tree, tvb, offset, tag_len, "}[%u]", *tag_no );
        else if (tag_is_context_specific(tag)) {
            ti = proto_tree_add_text(tree, tvb, offset, tag_len,
                "Context Tag: %u, Length/Value/Type: %u",
                *tag_no, *lvt);
        } else
            ti = proto_tree_add_text(tree, tvb, offset, tag_len,
                "Application Tag: %s, Length/Value/Type: %u",
                val_to_str(*tag_no,
                    BACnetApplicationTagNumber,
                    ASHRAE_Reserved_Fmt),
                    *lvt);

        subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
        
        proto_tree_add_item(subtree, hf_BACnetTagClass, tvb, offset, 1, ENC_BIG_ENDIAN);
        if (tag_is_extended_tag_number(tag)) {
            proto_tree_add_uint_format(subtree,
                    hf_BACnetContextTagNumber,
                    tvb, offset, 1, tag,
                    "Extended Tag Number");
            proto_tree_add_item(subtree,
                hf_BACnetExtendedTagNumber,
                tvb, offset + 1, 1, ENC_BIG_ENDIAN);
        } else {
            if (tag_is_context_specific(tag))
                proto_tree_add_item(subtree,
                    hf_BACnetContextTagNumber,
                    tvb, offset, 1, ENC_BIG_ENDIAN);
            else
                proto_tree_add_item(subtree,
                    hf_BACnetApplicationTagNumber,
                    tvb, offset, 1, ENC_BIG_ENDIAN);
        }
        if (tag_is_closing(tag) || tag_is_opening(tag))
            proto_tree_add_item(subtree,
                hf_BACnetNamedTag,
                tvb, offset, 1, ENC_BIG_ENDIAN);
        else if (tag_is_extended_value(tag)) {
            proto_tree_add_item(subtree,
                hf_BACnetNamedTag,
                tvb, offset, 1, ENC_BIG_ENDIAN);
            proto_tree_add_uint(subtree, hf_bacapp_tag_lvt,
                tvb, lvt_offset, lvt_len, *lvt);
        } else
            proto_tree_add_uint(subtree, hf_bacapp_tag_lvt,
                tvb, lvt_offset, lvt_len, *lvt);
    }

    return tag_len;
}
