static guint
fBitStringTag (tvbuff_t *tvb, proto_tree *tree, guint offset, const gchar *label)
{
    return fBitStringTagVS (tvb, tree, offset, label, NULL);
}
