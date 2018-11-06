static guint
fAddressBinding (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    offset = fObjectIdentifier (tvb, pinfo, tree, offset);
    return fAddress (tvb, tree, offset);
}
