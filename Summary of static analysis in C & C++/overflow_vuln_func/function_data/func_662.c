static gint dissect_asterix_fields (tvbuff_t *tvb, guint offset, proto_tree *tree, guint8 category, const AsterixField *current_uap[])
{
    guint i, j, size, start, len, inner_offset, fspec_len;
    guint64 counter;
    proto_item *asterix_field_item = NULL;
    proto_tree *asterix_field_tree = NULL;
    proto_item *asterix_field_item2 = NULL;
    proto_tree *asterix_field_tree2 = NULL;

    if (current_uap == NULL)
        return 0;
    
    for (i = 0, size = 0; current_uap[i] != NULL; i++) {
        start = asterix_field_offset (tvb, offset, current_uap, i);
        if (start > 0) {
            len = asterix_field_length (tvb, offset + start, current_uap[i]);
            size += len;
            if (current_uap[i]->type & COMPOUND) {
                asterix_field_item = proto_tree_add_item (tree, *current_uap[i]->hf, tvb, offset + start, len, ENC_NA);
                asterix_field_tree = proto_item_add_subtree (asterix_field_item, ett_asterix_subtree);
                fspec_len = asterix_fspec_len (tvb, offset + start);
                proto_tree_add_item (asterix_field_tree, hf_asterix_fspec, tvb, offset + start, fspec_len, ENC_NA);
                dissect_asterix_fields (tvb, offset + start, asterix_field_tree, category, (const AsterixField **)current_uap[i]->field);
            }
            else if (current_uap[i]->type & REPETITIVE) {
                asterix_field_item = proto_tree_add_item (tree, *current_uap[i]->hf, tvb, offset + start, len, ENC_NA);
                asterix_field_tree = proto_item_add_subtree (asterix_field_item, ett_asterix_subtree);
                for (j = 0, counter = 0; j < current_uap[i]->repetition_counter_size; j++) {
                    counter = (counter << 8) + tvb_get_guint8 (tvb, offset + start + j);
                }
                proto_tree_add_item (asterix_field_tree, hf_counter, tvb, offset + start, current_uap[i]->repetition_counter_size, ENC_BIG_ENDIAN);
                for (j = 0, inner_offset = 0; j < counter; j++, inner_offset += current_uap[i]->length) {
                    asterix_field_item2 = proto_tree_add_item (asterix_field_tree, *current_uap[i]->hf, tvb, offset + start + current_uap[i]->repetition_counter_size + inner_offset, current_uap[i]->length, ENC_NA);
                    asterix_field_tree2 = proto_item_add_subtree (asterix_field_item2, ett_asterix_subtree);
                    asterix_build_subtree (tvb, offset + start + current_uap[i]->repetition_counter_size + inner_offset, asterix_field_tree2, current_uap[i]);
                }
            }
            else {
                asterix_field_item = proto_tree_add_item (tree, *current_uap[i]->hf, tvb, offset + start, len, ENC_NA);
                asterix_field_tree = proto_item_add_subtree (asterix_field_item, ett_asterix_subtree);
                asterix_build_subtree (tvb, offset + start, asterix_field_tree, current_uap[i]);
            }
        }
    }
    return size;
}
