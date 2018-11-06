    header_tree = proto_tree_add_subtree(tree, tvb, hdr_start, offset - hdr_start, ett, \
                                                &header_item, header); \
    proto_tree_add_item(header_tree, hf_hdr, tvb, hdr_start, 1, ENC_NA); \
    if (val_id & 0x80) { / \
        offset++; \
        /

#define wkh_2_TextualValue                  / \
        / \
    } else if ((val_id == 0) || (val_id >= 0x20)) { / \
        val_str = (gchar *)tvb_get_stringz_enc(wmem_packet_scope(), tvb, val_start, (gint *)&val_len, ENC_ASCII); \
        offset = val_start + val_len; \
        /

#define wkh_2_TextualValueInv                   / \
        / \
    } else if ((val_id == 0) || (val_id >= 0x20)) { / \
        /tvb_get_stringz_enc(wmem_packet_scope(), tvb, val_start, (gint *)&val_len, ENC_ASCII); \
        offset = val_start + val_len; \
        /

#define wkh_3_ValueWithLength               / \
        / \
    } else { / \
        if (val_id == 0x1F) { / \
            val_len = tvb_get_guintvar(tvb, val_start + 1, &val_len_len); \
            val_len_len++; / \
        } else { / \
            val_len = tvb_get_guint8(tvb, offset); \
            val_len_len = 1; \
        } \
        offset += val_len_len + val_len; \
        /

#define wkh_4_End()                       / \
        / \
    } \
