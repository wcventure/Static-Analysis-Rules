static void
dissect_fhandle_data(tvbuff_t *tvb, int offset, packet_info *pinfo, proto_tree *tree, 
                     unsigned int fhlen, gboolean hidden, guint32 *hash)
{
	/
	if(nfs_fhandle_reqrep_matching && (!hidden) ){
		nfs_fhandle_data_t *old_fhd=NULL;

		if( !pinfo->fd->flags.visited ){
			nfs_fhandle_data_t fhd;

			/
			fhd.len=fhlen;
			fhd.fh=(const unsigned char *)tvb_get_ptr(tvb, offset, fhlen);
			old_fhd=store_nfs_file_handle(&fhd);

			/
			se_tree_insert32(nfs_fhandle_frame_table, pinfo->fd->num, old_fhd);
		}
	}

	/
	{
		guint32 fhhash;
		guint8 *fh_array;
		proto_item *fh_item = NULL;

		fh_array = tvb_get_string(tvb, offset, fhlen);
		fhhash = crc32_ccitt(fh_array, fhlen);
		g_free(fh_array);

		if(hidden){
			fh_item=proto_tree_add_uint(tree, hf_nfs_fh_hash, NULL, 0,
				0, fhhash);
			PROTO_ITEM_SET_HIDDEN(fh_item);
		} else {
			fh_item=proto_tree_add_uint(tree, hf_nfs_fh_hash, tvb, offset,
				fhlen, fhhash);
		}
		PROTO_ITEM_SET_GENERATED(fh_item);
		if(hash){
			*hash=fhhash;
		}
	}
	if(nfs_file_name_snooping){
		nfs_name_snoop_fh(pinfo, tree, tvb, offset, fhlen, hidden);
	}

	if(!hidden){
		tvbuff_t *fh_tvb;
		int real_length;

		proto_tree_add_text(tree, tvb, offset, 0, "decode type as: %s", 
			val_to_str_ext_const(default_nfs_fhandle_type, &names_fhtype_ext, "Unknown"));

		real_length=fhlen;
		if(default_nfs_fhandle_type != FHT_UNKNOWN && real_length<tvb_length_remaining(tvb, offset))
			real_length=tvb_length_remaining(tvb, offset);
		
		fh_tvb=tvb_new_subset(tvb, offset, real_length, fhlen);
		if(!dissector_try_uint(nfs_fhandle_table, default_nfs_fhandle_type, fh_tvb, pinfo, tree))
			dissect_fhandle_data_unknown(fh_tvb, pinfo, tree);
	}
}
