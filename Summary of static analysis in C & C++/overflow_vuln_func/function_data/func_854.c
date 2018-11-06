static void
fIHave (tvbuff_t *tvb, proto_tree *tree, guint *offset)
{
	/
	fApplicationTags (tvb, tree, offset, "DeviceIdentifier: ", NULL);

	/
	fApplicationTags (tvb, tree, offset, "ObjectIdentifier: ", NULL);

	/
	fApplicationTags (tvb, tree, offset, "ObjectName: ", NULL);

}
