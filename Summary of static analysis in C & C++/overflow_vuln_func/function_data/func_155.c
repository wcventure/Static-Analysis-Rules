static guint
fUTCTimeSynchronizationRequest  (tvbuff_t *tvb, proto_tree *tree, guint offset)
{
    if (tvb_reported_length_remaining(tvb, offset) <= 0)
        return offset;

    return fDateTime (tvb, tree, offset, "UTC-Time: ");
}
