static int
dissect_nfs4_acemask(tvbuff_t *tvb, int offset, proto_tree *tree)
{
	guint32 acemask;
	proto_item *acemask_item = NULL;
	proto_tree *acemask_tree = NULL;

	acemask = tvb_get_ntohl(tvb, offset);

	acemask_item = proto_tree_add_text(tree, tvb, offset, 4,
		"acemask: 0x%08x", acemask);

	if (acemask_item)
		acemask_tree = proto_item_add_subtree(acemask_item, ett_nfs4_acemask);

	if (acemask_tree)
	{
		if (acemask & ACE4_READ_DATA)
			proto_tree_add_text(acemask_tree, tvb, offset, 4,
				"ACE4_READ_DATA/ACE4_LIST_DIRECTORY (0x%08x)",
				ACE4_READ_DATA);

		if (acemask & ACE4_WRITE_DATA)
			proto_tree_add_text(acemask_tree, tvb, offset, 4,
				"ACE4_WRITE_DATA/ACE4_ADD_FILE (0x%08x)",
				ACE4_WRITE_DATA);

		if (acemask & ACE4_APPEND_DATA)
			proto_tree_add_text(acemask_tree, tvb, offset, 4,
				"ACE4_APPEND_DATA/ACE4_ADD_SUBDIRECTORY (0x%08x)",
				ACE4_APPEND_DATA);

		if (acemask & ACE4_READ_NAMED_ATTRS)
			proto_tree_add_text(acemask_tree, tvb, offset, 4,
				"ACE4_READ_NAMED_ATTRS (0x%08x)",
				ACE4_READ_NAMED_ATTRS);

		if (acemask & ACE4_WRITE_NAMED_ATTRS)
			proto_tree_add_text(acemask_tree, tvb, offset, 4,
				"ACE4_WRITE_NAMED_ATTRS (0x%08x)",
				ACE4_WRITE_NAMED_ATTRS);

		if (acemask & ACE4_EXECUTE)
			proto_tree_add_text(acemask_tree, tvb, offset, 4,
				"ACE4_EXECUTE (0x%08x)",
				ACE4_EXECUTE);

		if (acemask & ACE4_DELETE_CHILD)
			proto_tree_add_text(acemask_tree, tvb, offset, 4,
				"ACE4_DELETE_CHILD (0x%08x)",
				ACE4_DELETE_CHILD);

		if (acemask & ACE4_READ_ATTRIBUTES)
			proto_tree_add_text(acemask_tree, tvb, offset, 4,
				"ACE4_READ_ATTRIBUTES (0x%08x)",
				ACE4_READ_ATTRIBUTES);

		if (acemask & ACE4_WRITE_ATTRIBUTES)
			proto_tree_add_text(acemask_tree, tvb, offset, 4,
				"ACE4_WRITE_ATTRIBUTES (0x%08x)",
				ACE4_WRITE_ATTRIBUTES);

		if (acemask & ACE4_DELETE)
			proto_tree_add_text(acemask_tree, tvb, offset, 4,
				"ACE4_DELETE (0x%08x)",
				ACE4_DELETE);

		if (acemask & ACE4_READ_ACL)
			proto_tree_add_text(acemask_tree, tvb, offset, 4,
				"ACE4_READ_ACL (0x%08x)",
				ACE4_READ_ACL);

		if (acemask & ACE4_WRITE_ACL)
			proto_tree_add_text(acemask_tree, tvb, offset, 4,
				"ACE4_WRITE_ACL (0x%08x)",
				ACE4_WRITE_ACL);

		if (acemask & ACE4_WRITE_OWNER)
			proto_tree_add_text(acemask_tree, tvb, offset, 4,
				"ACE4_WRITE_OWNER (0x%08x)",
				ACE4_WRITE_OWNER);

		if (acemask & ACE4_SYNCHRONIZE)
			proto_tree_add_text(acemask_tree, tvb, offset, 4,
				"ACE4_SYNCHRONIZE (0x%08x)",
				ACE4_SYNCHRONIZE);
	}

	offset += 4;

	return offset;
}
