static void dissect_asterix_data_block (tvbuff_t *tvb, guint offset, proto_tree *tree, guint8 category, gint length)
{
    guint8 active_uap;
    int fspec_len, inner_offset, size, counter;
    proto_item *asterix_message_item = NULL;
    proto_tree *asterix_message_tree = NULL;
    
    for (counter = 1, inner_offset = 0; inner_offset < length; counter++) {
        active_uap = asterix_get_active_uap (tvb, offset + inner_offset, category);
        size = asterix_message_length (tvb, offset + inner_offset, category, active_uap);
        if (size > 0) {
            asterix_message_item = proto_tree_add_item (tree, hf_asterix_message, tvb, offset + inner_offset, size, ENC_NA);
            proto_item_append_text (asterix_message_item, ", #%02d, length: %d", counter, size);
            asterix_message_tree = proto_item_add_subtree (asterix_message_item, ett_asterix_message);
            fspec_len = asterix_fspec_len (tvb, offset + inner_offset);
            /
            proto_tree_add_item (asterix_message_tree, hf_asterix_fspec, tvb, offset + inner_offset, fspec_len, ENC_NA);
            
            size = dissect_asterix_fields (tvb, offset + inner_offset, asterix_message_tree, category, uap[category][active_uap]);
            
            inner_offset += size + fspec_len;
        }
        else {
            inner_offset = length;
        }
    }
}
