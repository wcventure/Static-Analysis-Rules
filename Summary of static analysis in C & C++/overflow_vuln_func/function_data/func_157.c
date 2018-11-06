static guint
fDateRange  (tvbuff_t *tvb, proto_tree *tree, guint offset)
{
    if (tvb_reported_length_remaining(tvb, offset) <= 0)
        return offset;
    offset = fDate (tvb,tree,offset,"Start Date: ");
    return fDate (tvb, tree, offset, "End Date: ");
}
