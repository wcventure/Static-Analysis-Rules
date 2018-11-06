static void dissect_asterix_packet (tvbuff_t *tvb, proto_tree *tree)
{
    guint i;
    guint8 category;
    guint16 length;
    proto_item *asterix_packet_item = NULL;
    proto_tree *asterix_packet_tree = NULL;

    for (i = 0; i < tvb_reported_length (tvb); i += length + 3) {
        category = tvb_get_guint8 (tvb, i);
        length = (tvb_get_guint8 (tvb, i + 1) << 8) + tvb_get_guint8 (tvb, i + 2) - 3; /
        / /

        asterix_packet_item = NULL;
        asterix_packet_tree = NULL;
        asterix_packet_item = proto_tree_add_item (tree, proto_asterix, tvb, i, length + 3, ENC_NA);
        proto_item_append_text (asterix_packet_item, ", Category %03d", category);
        asterix_packet_tree = proto_item_add_subtree (asterix_packet_item, ett_asterix);
        proto_tree_add_item (asterix_packet_tree, hf_asterix_category, tvb, i, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item (asterix_packet_tree, hf_asterix_length, tvb, i + 1, 2, ENC_BIG_ENDIAN);
        
        dissect_asterix_data_block (tvb, i + 3, asterix_packet_tree, category, length);
    }
}
