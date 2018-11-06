static int
dissect_group(tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree)
{
	int len,str_len;
	len=tvb_get_ntohl(tvb,offset);
	str_len=tvb_get_nstringz(tvb,offset+4,
		MAX_GROUP_NAME_LIST-5-group_names_len,
		group_name_list+group_names_len);
	if((group_names_len>=(MAX_GROUP_NAME_LIST-5))||(str_len<0)){
		strcpy(group_name_list+(MAX_GROUP_NAME_LIST-5),"...");
		group_names_len=MAX_GROUP_NAME_LIST-1;
	} else {
		group_names_len+=str_len;
		group_name_list[group_names_len++]=' ';
	}
	group_name_list[group_names_len]=0;

	offset = dissect_rpc_string(tvb, tree,
			hf_mount_groups_group, offset, NULL);

	return offset;
}
