static guint
get_value_length(tvbuff_t *tvb, guint offset, guint *byte_count)
{
    guint        field;

    field = tvb_get_guint8(tvb, offset++);
    if (field < 31)
        *byte_count = 1;
    else {                      /
        field = tvb_get_guintvar(tvb, offset, byte_count);
        (*byte_count)++;
    }
    return field;
}
