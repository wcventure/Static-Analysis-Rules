static void dissect_acl(const u_char *pd, int offset, frame_data *fd, proto_tree *tree)
{
	int pos, neg, acl;
	int n, i, bytes;
	u_char const *s;
	u_char const *end;
	char user[128];
	int curoffset;
	int soff,eoff;

	curoffset = offset;

	TRUNC(sizeof(guint32));
	bytes = pntohl(&pd[curoffset]);
	OUT_UINT(hf_afs_fs_acl_datasize);

	TRUNC(bytes);

	soff = curoffset;
	eoff = curoffset+bytes;

	s = &pd[soff];
	end = &pd[eoff];

	if (sscanf((char *) s, "%d %n", &pos, &n) != 1)
		return;
	s += n;
	TRUNC(1);
	proto_tree_add_uint(tree, hf_afs_fs_acl_count_positive, NullTVB, curoffset, n, pos);
	curoffset += n;

	if (sscanf((char *) s, "%d %n", &neg, &n) != 1)
		return;
	s += n;
	TRUNC(1);
	proto_tree_add_uint(tree, hf_afs_fs_acl_count_negative, NullTVB, curoffset, n, neg);
	curoffset += n;


	/

	for (i = 0; i < pos; i++) {
		if (sscanf((char *) s, "%s %d %n", user, &acl, &n) != 2)
			return;
		s += n;
		ACLOUT(user,1,acl,n);
		curoffset += n;
		TRUNC(1);
	}

	for (i = 0; i < neg; i++) {
		if (sscanf((char *) s, "%s %d %n", user, &acl, &n) != 2)
			return;
		s += n;
		ACLOUT(user,0,acl,n);
		curoffset += n;
		if (s > end)
			return;
	}
}
