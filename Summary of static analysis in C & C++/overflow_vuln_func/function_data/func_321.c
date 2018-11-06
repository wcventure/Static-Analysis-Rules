static int
dissect_nfs4_ace(tvbuff_t *tvb, int offset, packet_info *pinfo _U_,
		proto_tree *tree)
{
	proto_item* ace_item = NULL;
	proto_tree* ace_tree = NULL;
	proto_item *aceflag_item = NULL;
	proto_tree *aceflag_tree = NULL;
	guint32 aceflag4;

	if (tree) {
		ace_item = proto_tree_add_text(tree, tvb, offset, 4,
			"ACE");

		ace_tree = proto_item_add_subtree(ace_item, ett_nfs4_ace);
	}

	offset = dissect_rpc_uint32(tvb, ace_tree, hf_nfs4_acetype, offset);

	if (ace_tree) {
		aceflag4 = tvb_get_ntohl(tvb, offset);

		aceflag_item = proto_tree_add_text(ace_tree, tvb, offset, 4,
			"aceflag: 0x%08x", aceflag4);

		if (aceflag_item)
		{
			aceflag_tree = proto_item_add_subtree(aceflag_item, ett_nfs4_aceflag);

			if (aceflag_tree)
			{
				if (aceflag4 & ACE4_FILE_INHERIT_ACE)
					proto_tree_add_text(aceflag_tree, tvb, offset, 4,
						"ACE4_FILE_INHERIT_ACE (0x%08x)", ACE4_FILE_INHERIT_ACE);

				if (aceflag4 & ACE4_DIRECTORY_INHERIT_ACE)
					proto_tree_add_text(aceflag_tree, tvb, offset, 4,
						"ACE4_DIRECTORY_INHERIT_ACE (0x%08x)",
						 ACE4_DIRECTORY_INHERIT_ACE);

				if (aceflag4 & ACE4_INHERIT_ONLY_ACE)
					proto_tree_add_text(aceflag_tree, tvb, offset, 4,
						"ACE4_INHERIT_ONLY_ACE (0x%08x)",
						ACE4_INHERIT_ONLY_ACE);

				if (aceflag4 & ACE4_SUCCESSFUL_ACCESS_ACE_FLAG)
					proto_tree_add_text(aceflag_tree, tvb, offset, 4,
						"ACE4_SUCCESSFUL_ACCESS_ACE_FLAG (0x%08x)",
						ACE4_SUCCESSFUL_ACCESS_ACE_FLAG);

				if (aceflag4 & ACE4_FAILED_ACCESS_ACE_FLAG)
					proto_tree_add_text(aceflag_tree, tvb, offset, 4,
						"ACE4_FAILED_ACCESS_ACE_FLAG (0x%08x)",
						ACE4_FAILED_ACCESS_ACE_FLAG);

				if (aceflag4 & ACE4_IDENTIFIER_GROUP)
					proto_tree_add_text(aceflag_tree, tvb, offset, 4,
						"ACE4_IDENTIFIER_GROUP (0x%08x)",
						ACE4_IDENTIFIER_GROUP);

				if (aceflag4 & ACE4_INHERITED_ACE)
					proto_tree_add_text(aceflag_tree, tvb, offset, 4,
						"ACE4_INHERITED_ACE (0x%08x)",
						ACE4_INHERITED_ACE);
			}
		}
	}

	offset += 4;

	offset = dissect_nfs4_acemask(tvb, offset, ace_tree);

	offset = dissect_nfs_utf8string(tvb, offset, ace_tree, hf_nfs4_who, NULL);

	return offset;
}
