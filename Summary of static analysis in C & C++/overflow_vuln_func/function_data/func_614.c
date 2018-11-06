static gint
dissect_reply_afp_get_server_param(tvbuff_t *tvb, packet_info *pinfo _U_, proto_tree *tree, gint offset)
{
	guint8 num;
	guint8 len;
	guint8 flag;
	guint8 i;
  	proto_tree *sub_tree = NULL;
  	proto_item *item;

	if (!tree)
		return offset;

	print_date(tree, hf_afp_server_time,tvb, offset);
	offset += 4;

	num = tvb_get_guint8(tvb, offset);
	item = proto_tree_add_text(tree, tvb, offset, 1, "Volumes : %d", num);
	sub_tree = proto_item_add_subtree(item, ett_afp_server_vol);
	offset++;
		
	for (i = 0; i < num; i++) {
		const gchar *rep;
		
		item = proto_tree_add_text(sub_tree, tvb, offset, -1,"Volume");
		tree = proto_item_add_subtree(item, ett_afp_vol_list);

		flag = tvb_get_guint8(tvb, offset);
		proto_tree_add_text(tree, tvb, offset , 1,"Flags : 0x%02x", flag);
#if 0
		/
		hf_afp_vol_flag_passwd hf_afp_vol_flag_unix_priv 
#endif		
		offset++;

		len  = tvb_get_guint8(tvb, offset) +1;
		rep = get_name(tvb, offset, 2);
		proto_item_set_text(item, rep);
		proto_item_set_len(item, len +1);     

		proto_tree_add_item(tree, hf_afp_vol_name, tvb, offset, 1,FALSE);

		offset += len;
	}			
	return offset;
}
