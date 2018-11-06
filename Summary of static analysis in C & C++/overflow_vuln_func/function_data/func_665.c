static int asterix_field_length (tvbuff_t *tvb, guint offset, const AsterixField *field)
{
    guint size;
    guint64 count;
    guint8 i;
    
    size = 0;
    if (field->type & FIXED) {
        size = field->length;
    }
    else if (field->type & REPETITIVE) {
        for (i = 0, count = 0; i < field->repetition_counter_size && i < sizeof (count); i++)
            count = (count << 8) + tvb_get_guint8 (tvb, offset + i);
        size = (guint)(field->repetition_counter_size + count * field->length);
    }
    else if (field->type & FX) {
        for (size = field->length; tvb_get_guint8 (tvb, offset + size - 1) & 1; size += field->length);
    }
    else if (field->type & VAR) {
        for (i = 0, size = 0; i < field->header_length; i++) {
            size = (size << 8) + tvb_get_guint8 (tvb, offset + i);
        }
    }
    else if (field->type & COMPOUND) {
        /
        for (size = 0; tvb_get_guint8 (tvb, offset + size) & 1; size++);
        size++;

        for (i = 0; field->field[i] != NULL; i++) {
            if (asterix_field_exists (tvb, offset, i))
                size += asterix_field_length (tvb, offset + size, field->field[i]);
        }
    }
    return size;
}
