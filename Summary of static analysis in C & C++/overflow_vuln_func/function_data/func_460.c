static int
dissect_dfs_referral_flags(tvbuff_t *tvb, proto_tree *parent_tree, int offset)
{
	guint16 mask;
	proto_item *item;
	proto_tree *tree;

	mask = tvb_get_letohs(tvb, offset);

	if(parent_tree){
		item = proto_tree_add_text(parent_tree, tvb, offset, 2,
			"Flags: 0x%04x", mask);
		tree = proto_item_add_subtree(item, ett_smb_dfs_referral_flags);

		proto_tree_add_boolean(tree, hf_smb_dfs_referral_flags_strip,
			tvb, offset, 2, mask);
	}

	offset += 2;

	return offset;
}
