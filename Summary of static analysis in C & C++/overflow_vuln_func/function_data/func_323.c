static int
dissect_nfs4_fattrs(tvbuff_t *tvb, int offset, packet_info *pinfo, proto_tree *tree, int type)
{
	int attr_mask_offset = 0;
	guint8 i, j, num_bitmaps, count=0;
	guint32 attr_num;
	guint32 *bitmaps=NULL, bitmap, sl;
	gboolean first_attr=TRUE, no_idx=FALSE;

	proto_item *bitmap_item = NULL;
	proto_tree *bitmap_tree = NULL;
	proto_item *hitem = NULL;
	proto_item *attr_item = NULL;
	proto_tree *attr_tree = NULL;

	num_bitmaps = tvb_get_ntohl(tvb, offset);
	offset += 4;

	if (num_bitmaps > MAX_BITMAPS) {
		if (tree)
			proto_tree_add_text(tree, tvb, offset, 4, "Huge bitmap length: %u", num_bitmaps);
		THROW(ReportedBoundsError);
	}
	tvb_ensure_bytes_exist(tvb, offset, num_bitmaps * 4);

	if(num_bitmaps) {
		bitmaps = (guint32 *)ep_alloc(num_bitmaps * sizeof(guint32));
		attr_mask_offset = offset;

		/
		for (i = 0; i < num_bitmaps; i++) {
			bitmaps[i] = tvb_get_ntohl(tvb, attr_mask_offset + (i*4));
			if (bitmaps[i] > 0)
				count++;
		}
		/
		if(count <= 1)
			no_idx = TRUE;

		/
		offset += (num_bitmaps * 4) + (type==FATTR4_DISSECT_VALUES ? 4 : 0);
	}

	if (!tree
	&& type==FATTR4_BITMAP_ONLY)
		return offset;

	for (i = 0; i < num_bitmaps; i++)
	{
		bitmap = bitmaps[i];

		if (bitmap) {
			if (tree) {
				/
				if (no_idx)
					bitmap_item = proto_tree_add_uint_format(tree, hf_nfs4_attr_mask, tvb,
						attr_mask_offset, 4, bitmap, "Attr mask: 0x%08x", bitmap);
				else
					bitmap_item = proto_tree_add_uint_format(tree, hf_nfs4_attr_mask, tvb, 
						attr_mask_offset, 4, bitmap, "Attr mask[%u]: 0x%08x", i, bitmap);

				bitmap_tree = proto_item_add_subtree(bitmap_item, ett_nfs4_bitmap);
				first_attr = TRUE;

				/
				for(count=0; bitmap; bitmap >>= 1)
					count += (bitmap & 1);
				bitmap = bitmaps[i];
				hitem = proto_tree_add_text(bitmap_tree, tvb, attr_mask_offset, 4, "[%u attribute%s]", count,
					plurality(count, "", "s"));
				PROTO_ITEM_SET_HIDDEN(hitem);
			}
		} else {
			attr_mask_offset += 4;
			continue;
		}

		sl = 0x00000001;

		for(j = 0; j < 32; j++) {
			attr_num = 32*i + j;

			if (bitmap & sl) {
				if (bitmap_tree) {
					/
					proto_item_append_text (bitmap_tree, (first_attr ? " (%s" : ", %s"), 
						val_to_str(attr_num, names_fattr4, "Unknown: %u"));
					first_attr = FALSE;

					/
					attr_item = proto_tree_add_uint(bitmap_tree,
						((attr_num <= 11 || attr_num==19 || attr_num==75) ? hf_nfs4_reqd_attr: hf_nfs4_reco_attr),
						tvb, attr_mask_offset, 4, attr_num);
				}
				if (type==FATTR4_DISSECT_VALUES)
				{
					/
					if (attr_item)
						attr_tree = proto_item_add_subtree(attr_item, ett_nfs4_bitmap);

					switch(attr_num)
					{
					case FATTR4_SUPPORTED_ATTRS:
						offset = dissect_nfs4_fattrs(tvb, offset, pinfo, attr_tree, FATTR4_BITMAP_ONLY);
						break;

					case FATTR4_TYPE:
						if (attr_tree)
							proto_tree_add_item(attr_tree, hf_nfs4_ftype, tvb, offset, 4, 
								ENC_BIG_ENDIAN);
						offset += 4;
						break;

					case FATTR4_FH_EXPIRE_TYPE:
						offset = dissect_nfs4_fattr_fh_expire_type(tvb,	offset, attr_tree);
						break;

					case FATTR4_CHANGE:
						offset = dissect_rpc_uint64(tvb, attr_tree,	hf_nfs4_changeid, offset);
						break;

					case FATTR4_SIZE:
						offset = dissect_rpc_uint64(tvb, attr_tree, hf_nfs4_fattr_size, offset);
						break;

					case FATTR4_LINK_SUPPORT:
						offset = dissect_rpc_bool(tvb,
							attr_tree, hf_nfs4_fattr_link_support, offset);
						break;

					case FATTR4_SYMLINK_SUPPORT:
						offset = dissect_rpc_bool(tvb, attr_tree, hf_nfs4_fattr_symlink_support, offset);
						break;

					case FATTR4_NAMED_ATTR:
						offset = dissect_rpc_bool(tvb, attr_tree, hf_nfs4_fattr_named_attr, offset);
						break;

					case FATTR4_FSID:
						offset = dissect_nfs4_fsid(tvb, offset,	attr_tree, "fattr4_fsid");
						break;

					case FATTR4_UNIQUE_HANDLES:
						offset = dissect_rpc_bool(tvb, attr_tree, hf_nfs4_fattr_unique_handles,	offset);
						break;

					case FATTR4_LEASE_TIME:
						offset = dissect_rpc_uint32(tvb, attr_tree, hf_nfs4_fattr_lease_time, offset);
						break;

					case FATTR4_RDATTR_ERROR:
						offset = dissect_nfs4_status(tvb, offset, attr_tree, NULL);
						break;

					case FATTR4_ACL:
						offset = dissect_nfs4_fattr_acl(tvb, offset, pinfo, attr_tree);
						break;

					case FATTR4_ACLSUPPORT:
						offset = dissect_rpc_uint32(tvb, attr_tree, hf_nfs4_fattr_aclsupport, offset);
						break;

					case FATTR4_ARCHIVE:
						offset = dissect_rpc_bool(tvb, attr_tree, hf_nfs4_fattr_archive, offset);
						break;

					case FATTR4_CANSETTIME:
						offset = dissect_rpc_bool(tvb, attr_tree, hf_nfs4_fattr_cansettime, offset);
						break;

					case FATTR4_CASE_INSENSITIVE:
						offset = dissect_rpc_bool(tvb, attr_tree, hf_nfs4_fattr_case_insensitive, offset);
						break;

					case FATTR4_CASE_PRESERVING:
						offset = dissect_rpc_bool(tvb, attr_tree, hf_nfs4_fattr_case_preserving, offset);
						break;

					case FATTR4_CHOWN_RESTRICTED:
						offset = dissect_rpc_bool(tvb, attr_tree, hf_nfs4_fattr_chown_restricted, offset);
						break;

					case FATTR4_FILEID:
						offset = dissect_rpc_uint64(tvb, attr_tree, hf_nfs4_fattr_fileid, offset);
						break;

					case FATTR4_FILES_AVAIL:
						offset = dissect_rpc_uint64(tvb, attr_tree, hf_nfs4_fattr_files_avail, offset);
						break;

					case FATTR4_FILEHANDLE:
						offset = dissect_nfs4_fh(tvb, offset, pinfo, attr_tree, "fattr4_filehandle", NULL);
						break;

					case FATTR4_FILES_FREE:
						offset = dissect_rpc_uint64(tvb, attr_tree, hf_nfs4_fattr_files_free, offset);
						break;

					case FATTR4_FILES_TOTAL:
						offset = dissect_rpc_uint64(tvb, attr_tree, hf_nfs4_fattr_files_total, offset);
						break;

					case FATTR4_FS_LOCATIONS:
						offset = dissect_nfs4_fs_locations(tvb, pinfo, offset, attr_tree,
							"fattr4_fs_locations");
						break;

					case FATTR4_HIDDEN:
						offset = dissect_rpc_bool(tvb, attr_tree, hf_nfs4_fattr_hidden, offset);
						break;

					case FATTR4_HOMOGENEOUS:
						offset = dissect_rpc_bool(tvb, attr_tree, hf_nfs4_fattr_homogeneous, offset);
						break;

					case FATTR4_MAXFILESIZE:
						offset = dissect_rpc_uint64(tvb, attr_tree, hf_nfs4_fattr_maxfilesize, offset);
						break;

					case FATTR4_MAXLINK:
						offset = dissect_rpc_uint32(tvb, attr_tree, hf_nfs4_fattr_maxlink, offset);
						break;

					case FATTR4_MAXNAME:
						offset = dissect_rpc_uint32(tvb, attr_tree, hf_nfs4_fattr_maxname, offset);
						break;

					case FATTR4_MAXREAD:
						offset = dissect_rpc_uint64(tvb, attr_tree, hf_nfs4_fattr_maxread, offset);
						break;

					case FATTR4_MAXWRITE:
						offset = dissect_rpc_uint64(tvb, attr_tree, hf_nfs4_fattr_maxwrite, offset);
						break;

					case FATTR4_MIMETYPE:
						offset = dissect_nfs_utf8string(tvb, offset, attr_tree,	hf_nfs4_fattr_mimetype,
							NULL);
						break;

					case FATTR4_MODE:
						offset = dissect_nfs4_mode(tvb,	offset, attr_tree, "fattr4_mode");
						break;

					case FATTR4_NO_TRUNC:
						offset = dissect_rpc_bool(tvb, attr_tree, hf_nfs4_fattr_no_trunc, offset);
						break;

					case FATTR4_NUMLINKS:
						offset = dissect_rpc_uint32(tvb, attr_tree, hf_nfs4_fattr_numlinks, offset);
						break;

					case FATTR4_OWNER:
						offset = dissect_nfs_utf8string(tvb, offset, attr_tree,	hf_nfs4_fattr_owner,
							NULL);
						break;

					case FATTR4_OWNER_GROUP:
						offset = dissect_nfs_utf8string(tvb, offset, attr_tree,
							hf_nfs4_fattr_owner_group, NULL);
						break;

					case FATTR4_QUOTA_AVAIL_HARD:
						offset = dissect_rpc_uint64(tvb, attr_tree, hf_nfs4_fattr_quota_hard, offset);
						break;

					case FATTR4_QUOTA_AVAIL_SOFT:
						offset = dissect_rpc_uint64(tvb, attr_tree, hf_nfs4_fattr_quota_soft, offset);
						break;

					case FATTR4_QUOTA_USED:
						offset = dissect_rpc_uint64(tvb, attr_tree, hf_nfs4_fattr_quota_used, offset);
						break;

					case FATTR4_RAWDEV:
						offset = dissect_nfs4_specdata(tvb, offset, attr_tree);
						break;

					case FATTR4_SPACE_AVAIL:
						offset = dissect_rpc_uint64(tvb, attr_tree, hf_nfs4_fattr_space_avail, offset);
						break;

					case FATTR4_SPACE_FREE:
						offset = dissect_rpc_uint64(tvb, attr_tree, hf_nfs4_fattr_space_free, offset);
						break;

					case FATTR4_SPACE_TOTAL:
						offset = dissect_rpc_uint64(tvb, attr_tree, hf_nfs4_fattr_space_total, offset);
						break;

					case FATTR4_SPACE_USED:
						offset = dissect_rpc_uint64(tvb, attr_tree, hf_nfs4_fattr_space_used, offset);
						break;

					case FATTR4_SYSTEM:
						if (attr_tree)
							dissect_rpc_bool(tvb, attr_tree, hf_nfs4_fattr_system, offset);
						offset += 4;
						break;

					case FATTR4_TIME_ACCESS:
					case FATTR4_TIME_BACKUP:
					case FATTR4_TIME_CREATE:
					case FATTR4_TIME_DELTA:
					case FATTR4_TIME_METADATA:
					case FATTR4_TIME_MODIFY:
					case FATTR4_DIR_NOTIF_DELAY:
					case FATTR4_DIRENT_NOTIF_DELAY:
						if (attr_tree)
							dissect_nfs4_nfstime(tvb, offset, attr_tree);
						offset += 12;
						break;

					case FATTR4_TIME_ACCESS_SET:
					case FATTR4_TIME_MODIFY_SET:
						offset = dissect_nfs4_settime(tvb, offset, attr_tree, "settime4");
						break;

					case FATTR4_MOUNTED_ON_FILEID:
						offset = dissect_rpc_uint64(tvb, attr_tree, hf_nfs4_fattr_mounted_on_fileid,
									 offset);
						break;

					case FATTR4_FS_LAYOUT_TYPE:
						offset = dissect_nfs_fs_layout_type(tvb, attr_tree, offset);
						break;

					case FATTR4_LAYOUT_BLKSIZE:
						offset = dissect_rpc_uint32(tvb, attr_tree, hf_nfs4_fattr_layout_blksize,
									offset);
						break;

					case FATTR4_SECURITY_LABEL:
						offset = dissect_nfs4_security_label(tvb, attr_tree, offset);
						break;

					default:
						break;
					}
				}
			}
			sl <<= 1;
		} /

		if (bitmap_tree)
			proto_item_append_text(bitmap_tree, ")");
		attr_mask_offset += 4;
	} /

	return offset;
}
