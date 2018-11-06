static guint8 asterix_get_active_uap (tvbuff_t *tvb, guint offset, guint8 category)
{
    int i, inner_offset;
    AsterixField **current_uap;
    
    if (uap[category][1] != NULL) { /
        current_uap = (AsterixField **)uap[category][0];
        if (current_uap != NULL) {
            inner_offset = asterix_fspec_len (tvb, offset);
            for (i = 0; current_uap[i] != NULL; i++) {
                if (asterix_field_exists (tvb, offset, i)) {
                    if (current_uap[i]->type & UAP) {
                        return tvb_get_guint8 (tvb, offset + inner_offset) >> 7;
                    }
                    inner_offset += asterix_field_length (tvb, offset + inner_offset, current_uap[i]);
                }
            }
        }
    }
    return 0;
}
