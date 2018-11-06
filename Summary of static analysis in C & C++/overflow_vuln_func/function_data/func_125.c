static guint
fSessionKey (tvbuff_t *tvb, proto_tree *tree, guint offset)
{
    offset = fOctetString (tvb,tree,offset,"session key: ", 8);
    return fAddress (tvb,tree,offset);
}
