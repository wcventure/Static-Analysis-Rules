static tvbuff_t *
dissect_6lowpan_frag(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, gboolean first)
{
    gint                offset = 0;
    gint                frag_size;
    guint16             dgram_size;
    guint16             dgram_tag;
    guint8              dgram_offset = 0;
    proto_tree *        frag_tree = NULL;
    proto_item *        ti = NULL;
    /
    tvbuff_t *          new_tvb = NULL;
    fragment_data *     frag_data = NULL;
    gboolean            save_fragmented;

    /
    if (tree) {
        ti = proto_tree_add_text(tree, tvb, offset, 0, "Fragmentation Header");
        frag_tree = proto_item_add_subtree(ti, ett_6lowpan_frag);
    }

    /
    dgram_size = tvb_get_bits16(tvb, (offset * 8) + LOWPAN_PATTERN_FRAG_BITS, LOWPAN_FRAG_DGRAM_SIZE_BITS, FALSE);
    if (tree) {
        proto_tree_add_bits_item(frag_tree, hf_6lowpan_pattern, tvb, offset * 8, LOWPAN_PATTERN_FRAG_BITS, FALSE);
        proto_tree_add_uint(frag_tree, hf_6lowpan_frag_dgram_size, tvb, offset, sizeof(guint16), dgram_size);
    }
    offset += sizeof(guint16);

    /
    dgram_tag = tvb_get_ntohs(tvb, offset);
    if (tree) {
        proto_tree_add_uint(frag_tree, hf_6lowpan_frag_dgram_tag, tvb, offset, sizeof(guint16), dgram_tag);
    }
    offset += sizeof(guint16);

    if (!first) {
        dgram_offset = tvb_get_guint8(tvb, offset) * 8;
        if (tree) {
            proto_tree_add_uint(frag_tree, hf_6lowpan_frag_dgram_offset, tvb, offset, sizeof(guint8), dgram_offset);
        }
        offset += sizeof(guint8);
    }

    /
    frag_size = tvb_length_remaining(tvb, offset);
    if (tree) {
        proto_item_set_end(ti, tvb, offset);
    }

    /
    save_fragmented = pinfo->fragmented;
    pinfo->fragmented = TRUE;
    frag_data = fragment_add_check(tvb, offset, pinfo, dgram_tag,
                    lowpan_fragment_table, lowpan_reassembled_table,
                    dgram_offset, frag_size, ((dgram_offset + frag_size) < dgram_size));

    /
    new_tvb = process_reassembled_data(tvb, offset, pinfo,
                    "Reassembled Message", frag_data, &lowpan_frag_items,
                    NULL, tree);

    /
    if (new_tvb) {
        return new_tvb;
    }
    /
    else if (first) {
        return tvb_new_subset(tvb, offset, -1, dgram_size);
    }
    /
    else {
        tvbuff_t *      data_tvb = tvb_new_subset(tvb, offset, -1, dgram_size);
        call_dissector(data_handle, data_tvb, pinfo, proto_tree_get_root(tree));
        return NULL;
    }
} /
