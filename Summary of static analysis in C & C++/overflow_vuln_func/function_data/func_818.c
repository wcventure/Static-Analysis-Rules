static void
fDeleteObject (tvbuff_t *tvb, proto_tree *tree, guint *offset)
{
	fObjectIdentifier (tvb, tree, offset, "BACnetObjectIdentifier: ");
}
