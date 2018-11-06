static void
format_amqp_0_10_sequence_set(tvbuff_t *tvb, guint offset, guint length,
                              proto_item *item)
{
    guint values;

    /
    if ((length % 4) != 0) {
        proto_item_append_text(item, "Invalid sequence set length %u",
                               length);
    }

    values = length / 4;
    /
    if ((values % 2) != 0) {
        proto_item_append_text(item, "Invalid sequence set value count %u",
                               values);
    }
    proto_item_append_text(item, " [");
    while(values > 0) {
        proto_item_append_text(item, "(%u, %u)%s",
                               tvb_get_ntohl(tvb, offset),
                               tvb_get_ntohl(tvb, offset + 4),
                               values > 2 ? ", " : "");
        offset += 8;
        values -= 2;
    }
    proto_item_append_text(item, "]");
}
