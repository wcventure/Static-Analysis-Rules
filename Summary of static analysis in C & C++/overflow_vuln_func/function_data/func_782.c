int
dissect_nt_sec_desc(tvbuff_t *tvb, int offset, packet_info *pinfo,
		    proto_tree *parent_tree, guint8 *drep, int len, 
		    struct access_mask_info *ami)
{
	proto_item *item = NULL;
	proto_tree *tree = NULL;
	guint16 revision;
	int old_offset = offset;
	guint32 owner_sid_offset;
	guint32 group_sid_offset;
	guint32 sacl_offset;
	guint32 dacl_offset;

	if(parent_tree){
		item = proto_tree_add_text(parent_tree, tvb, offset, len,
					   "NT Security Descriptor");
		tree = proto_item_add_subtree(item, ett_nt_sec_desc);
	}

	/
	revision = tvb_get_letohs(tvb, offset);
	proto_tree_add_uint(tree, hf_nt_sec_desc_revision,
		tvb, offset, 2, revision);
	offset += 2;


	switch(revision){
	case 1:  /
	  /
	  offset = dissect_nt_sec_desc_type(tvb, offset, tree);

	  /
	  owner_sid_offset = tvb_get_letohl(tvb, offset);
	  proto_tree_add_text(tree, tvb, offset, 4, "Offset to owner SID: %u", owner_sid_offset);
	  offset += 4;

	  /
	  group_sid_offset = tvb_get_letohl(tvb, offset);
	  proto_tree_add_text(tree, tvb, offset, 4, "Offset to group SID: %u", group_sid_offset);
	  offset += 4;

	  /
	  sacl_offset = tvb_get_letohl(tvb, offset);
	  proto_tree_add_text(tree, tvb, offset, 4, "Offset to SACL: %u", sacl_offset);
	  offset += 4;

	  /
	  dacl_offset = tvb_get_letohl(tvb, offset);
	  proto_tree_add_text(tree, tvb, offset, 4, "Offset to DACL: %u", dacl_offset);
	  offset += 4;

	  /
	  if(owner_sid_offset){
	    if (len == -1)
	      offset = dissect_nt_sid(tvb, offset, tree, "Owner", NULL, -1);
	    else
	      dissect_nt_sid(
		      tvb, old_offset+owner_sid_offset, tree, "Owner", NULL, -1);
	  }

	  /
	  if(group_sid_offset){
	    dissect_nt_sid(
		    tvb, old_offset+group_sid_offset, tree, "Group", NULL, -1);
	  }

	  /
	  if(sacl_offset){
	    dissect_nt_acl(tvb, old_offset+sacl_offset, pinfo, tree, 
			   drep, "System (SACL)", ami);
	  }

	  /
	  if(dacl_offset){
	    dissect_nt_acl(tvb, old_offset+dacl_offset, pinfo, tree, 
			   drep, "User (DACL)", ami);
	  }

	}

	return offset+len;
}
