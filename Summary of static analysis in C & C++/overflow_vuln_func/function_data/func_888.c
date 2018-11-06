static guint
get_amqp_0_10_message_len(packet_info *pinfo _U_, tvbuff_t *tvb, int offset)
{
    /
    if (tvb_memeql(tvb, offset, "AMQP", 4) == 0)
        return 8;

    return (guint) tvb_get_ntohs(tvb, offset + 2);
}
