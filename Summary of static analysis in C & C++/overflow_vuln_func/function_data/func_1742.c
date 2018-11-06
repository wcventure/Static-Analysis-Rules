void radius_ipaddr(radius_attr_info_t* a, proto_tree* tree, packet_info *pinfo _U_, tvbuff_t* tvb, int offset, int len, proto_item* avp_item) {
	guint32 ip;
	gchar buf[16];

	if (len != 4) {
		proto_item_append_text(avp_item, "[wrong length for IP address]");
		return;
	}

	ip=tvb_get_ipv4(tvb,offset);

	proto_tree_add_item(tree, a->hf, tvb, offset, len, FALSE);

	ip_to_str_buf((guint8 *)&ip, buf);
	proto_item_append_text(avp_item, "%s", buf);
}
