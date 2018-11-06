                      proto_item *item)
{
    proto_item     *map_tree;
    guint           namelen, size;
    guint8          type;
    const char     *name;
    const char     *amqp_typename;
    const char     *value;
    guint32         field_count;
    type_formatter  formatter;

    map_tree = proto_item_add_subtree(item, ett_amqp_0_10_map);
    field_count = tvb_get_ntohl(tvb, offset);
    AMQP_INCREMENT(offset, 4, bound);
    length -= 4;
    proto_item_append_text(item, " (%d entries)", field_count);
    while ((field_count > 0) && (length > 0)) {
        guint field_length = 0;
        guint field_start = offset;
        namelen = tvb_get_guint8(tvb, offset);
        AMQP_INCREMENT(offset, 1, bound);
        length -= 1;
        name = (char*) tvb_get_string_enc(wmem_packet_scope(), tvb, offset, namelen, ENC_UTF_8|ENC_NA);
        AMQP_INCREMENT(offset, namelen, bound);
        length -= namelen;
        type = tvb_get_guint8(tvb, offset);
        AMQP_INCREMENT(offset, 1, bound);
        length -= 1;
        if (get_amqp_0_10_type_formatter(type, &amqp_typename, &formatter, &size)) {
            field_length = formatter(tvb, offset, bound, size, &value); /
            proto_tree_add_none_format(map_tree,
                                       hf_amqp_field,
                                       tvb,
                                       field_start,
                                       1 + namelen + 1 + field_length,
                                       "%s (%s): %s",
                                       name, amqp_typename, value);
            AMQP_INCREMENT(offset, field_length, bound);
            length -= field_length;
        }
        else {  /
            guint size_field_len = 0;

            switch (type) {
            case AMQP_0_10_TYPE_MAP:
            case AMQP_0_10_TYPE_LIST:
            case AMQP_0_10_TYPE_ARRAY:
                field_length = tvb_get_ntohl(tvb, offset);
                size_field_len = 4;
                proto_tree_add_none_format(map_tree, hf_amqp_field,
                                           tvb, field_start, (1 + namelen + + 1 + 4 + field_length),
                                           "%s (composite): %d bytes",
                                           name, field_length);
                break;

            default: {   /
                guint temp = 1 << ((type & 0x70) >> 4);  /
                amqp_typename = "unimplemented type";

                /
                if ((type & 0x80) == 0) {
                    field_length = temp;  /
                }
                else if ((type & 0xc0) == 0xc0) {
                    field_length = 5;
                }
                else if ((type & 0xd0) == 0xd0) {
                    field_length = 9;
                }
                else if ((type & 0xf0) == 0xf0) {
                    field_length = 0;
                }

                /
                else if ((type & 0x80) == 0x80) {
                    size_field_len = temp;
                    switch (size_field_len) {
                    case 1:
                        field_length = tvb_get_guint8(tvb, offset);
                        break;
                    case 2:
                        field_length = tvb_get_ntohs(tvb, offset);
                        break;
                    case 4:
                        field_length = tvb_get_ntohl(tvb, offset);
                        break;
                    default:
                        field_length = 1;    /
                        amqp_typename = "reserved";
                        break;
                    }
                }
                else {
                    DISSECTOR_ASSERT_NOT_REACHED();
                }
                proto_tree_add_none_format(map_tree, hf_amqp_field,
                                           tvb, field_start, 1 + namelen + 1 + size_field_len + field_length,
                                           "%s (%s): (value field length: %d bytes)",
                                           name, amqp_typename, field_length);
            } /
            } /

            AMQP_INCREMENT(offset, (size_field_len + field_length), bound);
            length -= (size_field_len + field_length);
        }

        field_count -= 1;
    }
}
