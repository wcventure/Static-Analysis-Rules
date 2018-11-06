static void asterix_build_subtree (tvbuff_t *tvb, guint offset, proto_tree *parent, const AsterixField *field)
{
    gint i, inner_offset;
    guint8 go_on;
    gint64 value;
    char *str_buffer = NULL;
    
    if (field->part != NULL) {
        for (i = 0, inner_offset = 0, go_on = 1; go_on && field->part[i] != NULL; i++) {
            value = 0L;
            value = tvb_get_bits64 (tvb, offset * 8 + inner_offset, field->part[i]->bit_length, ENC_BIG_ENDIAN);
            if (field->part[i]->hf != NULL) {
                switch (field->part[i]->type) {
                    case FIELD_PART_FX:
                        if (!value) go_on = 0;
                    case FIELD_PART_INT:
                    case FIELD_PART_UINT:
                    case FIELD_PART_HEX:
                    case FIELD_PART_ASCII:
                    case FIELD_PART_SQUAWK:
                        proto_tree_add_item (parent, *field->part[i]->hf, tvb, offset + inner_offset / 8, byte_length (field->part[i]->bit_length), ENC_BIG_ENDIAN);
                        break;
                    case FIELD_PART_FLOAT:
                        twos_complement (&value, field->part[i]->bit_length);
                    case FIELD_PART_UFLOAT:
                        if (field->part[i]->format_string != NULL)
                            proto_tree_add_double_format_value (parent, *field->part[i]->hf, tvb, offset + inner_offset / 8, byte_length (field->part[i]->bit_length), value * field->part[i]->scaling_factor, field->part[i]->format_string, value * field->part[i]->scaling_factor);
                        else
                            proto_tree_add_double (parent, *field->part[i]->hf, tvb, offset + inner_offset / 8, byte_length (field->part[i]->bit_length), value * field->part[i]->scaling_factor);
                        break;
                    case FIELD_PART_CALLSIGN:
                        str_buffer = (char *)wmem_alloc (wmem_packet_scope (), 9);
                        str_buffer[0] = '\0';
                        g_snprintf (str_buffer, 8, "%c%c%c%c%c%c%c%c, ", AISCode[(value >> 42) & 63],
                                                                         AISCode[(value >> 36) & 63],
                                                                         AISCode[(value >> 30) & 63],
                                                                         AISCode[(value >> 24) & 63],
                                                                         AISCode[(value >> 18) & 63],
                                                                         AISCode[(value >> 12) & 63],
                                                                         AISCode[(value >> 6) & 63],
                                                                         AISCode[value & 63]);
                        proto_tree_add_string (parent, *field->part[i]->hf, tvb, offset + inner_offset / 8, byte_length (field->part[i]->bit_length), str_buffer);
                        break;
                }
            }
            inner_offset += field->part[i]->bit_length;
        }
    } /    
}
