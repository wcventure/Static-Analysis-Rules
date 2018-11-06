static guint
get_encoded_strval(tvbuff_t *tvb, guint offset, const char **strval)
{
    guint        field;
    guint        length;
    guint        count;

    field = tvb_get_guint8(tvb, offset);

    if (field < 32) {
        length = get_value_length(tvb, offset, &count);
        if (length < 2) {
            *strval = "";
        } else {
            /
            *strval = (char *)tvb_get_string_enc(wmem_packet_scope(), tvb, offset + count + 1, length - 1, ENC_ASCII);
        }
        return count + length;
    } else
        return get_text_string(tvb, offset, strval);
}
