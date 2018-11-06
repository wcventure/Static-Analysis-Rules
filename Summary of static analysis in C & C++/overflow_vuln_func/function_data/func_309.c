static void
dissect_fhandle_data_SVR4(tvbuff_t* tvb, packet_info *pinfo _U_, proto_tree *tree)
{
	gboolean	little_endian;	/
	gboolean	have_flag;	/
	gboolean	found;		/
	guint32		nof=0;
	guint32		len1;
	guint32		len2;
	guint32		fhlen;		/

	/
	little_endian = FALSE;

	/
	have_flag = FALSE;

	/
	found = FALSE;

	/
	fhlen = tvb_reported_length(tvb);

	/
	len1 = tvb_get_letohs(tvb, 8);
	if (tvb_bytes_exist(tvb, 10+len1, 2)) {
		len2 = tvb_get_letohs(tvb, 10+len1);

		if (12+len1+len2 == fhlen) {
			little_endian = TRUE;
			have_flag = FALSE;
			found = TRUE;
		}
		if (16+len1+len2 == fhlen) {
			little_endian = TRUE;
			have_flag = TRUE;
			found = TRUE;
		}
	}

	if (!found) {
		/
		len1 = tvb_get_ntohs(tvb, 8);
		if (tvb_bytes_exist(tvb, 10+len1, 2)) {
			len2 = tvb_get_ntohs(tvb, 10+len1);

			if (12+len1+len2 == fhlen) {
				little_endian = FALSE;
				have_flag = FALSE;
			}
			if (16+len1+len2 == fhlen) {
				little_endian = FALSE;
				have_flag = TRUE;
			}
		}
	}

	if (tree) 
		proto_tree_add_boolean(tree, hf_nfs_fh_endianness, tvb,	0, fhlen, little_endian);
	
	/

	/
	{
	guint32 fsid_O;
	guint32 fsid_L;
	guint32 temp;
	guint32 fsid_major;
	guint32 fsid_minor;

	fsid_O = nof;
	fsid_L = 4;
	if (little_endian)
		temp = tvb_get_letohl(tvb, fsid_O);
	else
		temp = tvb_get_ntohl(tvb, fsid_O);
	fsid_major = ( temp>>18 ) &  0x3fff; /
	fsid_minor = ( temp     ) & 0x3ffff; /
	
	if (tree) {
		proto_item* fsid_item = NULL;
		proto_tree* fsid_tree = NULL;

		fsid_item = proto_tree_add_text(tree, tvb,
			fsid_O, fsid_L,
			"file system ID: %d,%d", fsid_major, fsid_minor);
		fsid_tree = proto_item_add_subtree(fsid_item,
					ett_nfs_fh_fsid);
		if (little_endian) {
			proto_tree_add_uint(fsid_tree, hf_nfs_fh_fsid_major,
					tvb, fsid_O+2, 2, fsid_major);
			proto_tree_add_uint(fsid_tree, hf_nfs_fh_fsid_minor,
					tvb, fsid_O,   3, fsid_minor);
		}
		else {
			proto_tree_add_uint(fsid_tree, hf_nfs_fh_fsid_major,
					tvb, fsid_O,   2, fsid_major);
			proto_tree_add_uint(fsid_tree, hf_nfs_fh_fsid_minor,
					tvb, fsid_O+1, 3, fsid_minor);
		}
	}
	nof = fsid_O + fsid_L;
	}

	/
	{
	guint32 fstype_O;
	guint32 fstype_L;
	guint32 fstype;

	fstype_O = nof;
	fstype_L = 4;
	if (little_endian)
		fstype = tvb_get_letohl(tvb, fstype_O);
	else
		fstype = tvb_get_ntohl(tvb, fstype_O);
	if (tree) {
		proto_tree_add_uint(tree, hf_nfs_fh_fstype, tvb,
			fstype_O, fstype_L, fstype);
	}
	nof = fstype_O + fstype_L;
	}

	/
	{
	guint32 fn_O;
	guint32 fn_len_O;
	guint32 fn_len_L;
	guint32 fn_len;
	guint32 fn_data_O;
	guint32 fn_data_inode_O;
	guint32 fn_data_inode_L;
	guint32 inode;
	guint32 fn_data_gen_O;
	guint32 fn_data_gen_L;
	guint32 gen;
	guint32 fn_L;

	fn_O = nof;
	fn_len_O = fn_O;
	fn_len_L = 2;
	if (little_endian)
		fn_len = tvb_get_letohs(tvb, fn_len_O);
	else
		fn_len = tvb_get_ntohs(tvb, fn_len_O);
	fn_data_O = fn_O + fn_len_L;
	fn_data_inode_O = fn_data_O + 2;
	fn_data_inode_L = 4;
	if (little_endian)
		inode = tvb_get_letohl(tvb, fn_data_inode_O);
	else
		inode = tvb_get_ntohl(tvb, fn_data_inode_O);
	if (little_endian)
		fn_data_gen_O = fn_data_inode_O + fn_data_inode_L;
	else
		fn_data_gen_O = fn_data_inode_O + fn_data_inode_L;
	fn_data_gen_L = 4;
	if (little_endian)
		gen = tvb_get_letohl(tvb, fn_data_gen_O);
	else
		gen = tvb_get_ntohl(tvb, fn_data_gen_O);
	fn_L = fn_len_L + fn_len;
	if (tree) {
		proto_item* fn_item = NULL;
		proto_tree* fn_tree = NULL;

		fn_item = proto_tree_add_uint(tree, hf_nfs_fh_fn, tvb,
			fn_O, fn_L, inode);
		fn_tree = proto_item_add_subtree(fn_item,
					ett_nfs_fh_fn);
		proto_tree_add_uint(fn_tree, hf_nfs_fh_fn_len,
				tvb, fn_len_O, fn_len_L, fn_len);
		proto_tree_add_uint(fn_tree, hf_nfs_fh_fn_inode,
				tvb, fn_data_inode_O, fn_data_inode_L, inode);
		proto_tree_add_uint(fn_tree, hf_nfs_fh_fn_generation,
				tvb, fn_data_gen_O, fn_data_gen_L, gen);
	}
	nof = fn_O + fn_len_L + fn_len;
	}

	/
	{
	guint32 xfn_O;
	guint32 xfn_len_O;
	guint32 xfn_len_L;
	guint32 xfn_len;
	guint32 xfn_data_O;
	guint32 xfn_data_inode_O;
	guint32 xfn_data_inode_L;
	guint32 xinode;
	guint32 xfn_data_gen_O;
	guint32 xfn_data_gen_L;
	guint32 xgen;
	guint32 xfn_L;

	xfn_O = nof;
	xfn_len_O = xfn_O;
	xfn_len_L = 2;
	if (little_endian)
		xfn_len = tvb_get_letohs(tvb, xfn_len_O);
	else
		xfn_len = tvb_get_ntohs(tvb, xfn_len_O);
	xfn_data_O = xfn_O + xfn_len_L;
	xfn_data_inode_O = xfn_data_O + 2;
	xfn_data_inode_L = 4;
	if (little_endian)
		xinode = tvb_get_letohl(tvb, xfn_data_inode_O);
	else
		xinode = tvb_get_ntohl(tvb, xfn_data_inode_O);
	xfn_data_gen_O = xfn_data_inode_O + xfn_data_inode_L;
	xfn_data_gen_L = 4;
	if (little_endian)
		xgen = tvb_get_letohl(tvb, xfn_data_gen_O);
	else
		xgen = tvb_get_ntohl(tvb, xfn_data_gen_O);
	xfn_L = xfn_len_L + xfn_len;
	if (tree) {
		proto_item* xfn_item = NULL;
		proto_tree* xfn_tree = NULL;

		xfn_item = proto_tree_add_uint(tree, hf_nfs_fh_xfn, tvb,
			xfn_O, xfn_L, xinode);
		xfn_tree = proto_item_add_subtree(xfn_item,
					ett_nfs_fh_xfn);
		proto_tree_add_uint(xfn_tree, hf_nfs_fh_xfn_len,
				tvb, xfn_len_O, xfn_len_L, xfn_len);
		proto_tree_add_uint(xfn_tree, hf_nfs_fh_xfn_inode,
				tvb, xfn_data_inode_O, xfn_data_inode_L, xinode);
		proto_tree_add_uint(xfn_tree, hf_nfs_fh_xfn_generation,
				tvb, xfn_data_gen_O, xfn_data_gen_L, xgen);
	}
	nof = xfn_O + xfn_len_L + xfn_len;
	}

	/
	if (have_flag) {
		guint32	flag_O;
		guint32	flag_L;
		guint32	flag_value;

		flag_O = nof;
		flag_L = 4;
		if (little_endian)
			flag_value = tvb_get_letohl(tvb, flag_O);
		else
			flag_value = tvb_get_ntohl(tvb, flag_O);
		if (tree) {
			proto_tree_add_uint(tree, hf_nfs_fh_flag, tvb,
					    flag_O, flag_L, flag_value);
		}
	}
}
