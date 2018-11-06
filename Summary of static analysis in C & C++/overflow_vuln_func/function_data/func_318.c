static int
dissect_nfs3_fsinfo_reply(tvbuff_t *tvb, int offset, packet_info *pinfo _U_,
			  proto_tree* tree)
{
	guint32 status;
	proto_item*	properties_item = NULL;
	proto_tree*	properties_tree = NULL;
	const char *err;

	offset = dissect_nfs3_status(tvb, offset, tree, &status);
	switch (status) {
		case 0:
			offset = dissect_nfs3_post_op_attr(tvb, offset, pinfo, tree,
				"obj_attributes");
			proto_tree_add_item(tree, hf_nfs3_fsinfo_rtmax, tvb, offset, 4, ENC_BIG_ENDIAN);
			offset += 4;
			proto_tree_add_item(tree, hf_nfs3_fsinfo_rtpref, tvb, offset, 4, ENC_BIG_ENDIAN);
			offset += 4;
			proto_tree_add_item(tree, hf_nfs3_fsinfo_rtmult, tvb, offset, 4, ENC_BIG_ENDIAN);
			offset += 4;
			proto_tree_add_item(tree, hf_nfs3_fsinfo_wtmax, tvb, offset, 4, ENC_BIG_ENDIAN);
			offset += 4;
			proto_tree_add_item(tree, hf_nfs3_fsinfo_wtpref, tvb, offset, 4, ENC_BIG_ENDIAN);
			offset += 4;
			proto_tree_add_item(tree, hf_nfs3_fsinfo_wtmult, tvb, offset, 4, ENC_BIG_ENDIAN);
			offset += 4;
			proto_tree_add_item(tree, hf_nfs3_fsinfo_dtpref, tvb, offset, 4, ENC_BIG_ENDIAN);
			offset += 4;

			offset = dissect_rpc_uint64(tvb, tree,	hf_nfs3_fsinfo_maxfilesize, offset);
			offset = dissect_nfstime3(tvb, offset, tree, hf_nfs_dtime, hf_nfs_dtime_sec, 
				hf_nfs_dtime_nsec);

			if (tree) {
				properties_item = proto_tree_add_item(tree, hf_nfs3_fsinfo_properties,
				    tvb, offset, 4, ENC_BIG_ENDIAN);

				properties_tree = proto_item_add_subtree(properties_item, ett_nfs3_fsinfo_properties);

				proto_tree_add_item(properties_tree, hf_nfs3_fsinfo_properties_setattr, tvb,
					offset, 4, ENC_BIG_ENDIAN);
				proto_tree_add_item(properties_tree, hf_nfs3_fsinfo_properties_pathconf, tvb, 
					offset, 4, ENC_BIG_ENDIAN);
				proto_tree_add_item(properties_tree, hf_nfs3_fsinfo_properties_symlinks, tvb, 
					offset, 4, ENC_BIG_ENDIAN);
				proto_tree_add_item(properties_tree, hf_nfs3_fsinfo_properties_hardlinks, tvb, 
					offset, 4, ENC_BIG_ENDIAN);
			}
			offset += 4;

			proto_item_append_text(tree, ", FSINFO Reply");
		break;
		default:
			offset = dissect_nfs3_post_op_attr(tvb, offset, pinfo, tree,
				"obj_attributes");

			err = val_to_str_ext(status, &names_nfs3_status_ext, "Unknown error: %u");
			col_append_fstr(pinfo->cinfo, COL_INFO," Error: %s", err);
			proto_item_append_text(tree, ", FSINFO Reply  Error: %s", err);
		break;
	}

	return offset;
}
