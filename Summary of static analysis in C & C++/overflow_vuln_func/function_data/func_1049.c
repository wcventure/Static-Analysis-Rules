static gint add_message(tvbuff_t * tvb, gint offset, proto_tree * tree,
			GString * info)
{
	guint16 descriptor_id, message_id, data_len;
	proto_item *msg;
	proto_tree *msg_tree;
	const gchar *descriptor;

	descriptor_id = tvb_get_ntohs(tvb, offset);
	message_id = tvb_get_ntohs(tvb, offset + 2);
	data_len = tvb_get_ntohs(tvb, offset + 4) * 2;

	/
	descriptor = val_to_str(descriptor_id, descriptors, "Unknown (%u)");
	if (descriptor_id == ACK)
		msg = proto_tree_add_none_format(tree,
						 hf_armagetronad_msg_subtree,
						 tvb, offset, data_len + 6,
						 "ACK %d messages",
						 data_len / 2);
	else
		msg = proto_tree_add_none_format(tree,
						 hf_armagetronad_msg_subtree,
						 tvb, offset, data_len + 6,
						 "Message 0x%04x [%s]",
						 message_id, descriptor);

	msg_tree = proto_item_add_subtree(msg, ett_message);

	/
	proto_tree_add_item(msg_tree, hf_armagetronad_descriptor_id, tvb,
			    offset, 2, FALSE);
	if (info)
		g_string_sprintfa(info, "%s, ", descriptor);

	/
	proto_tree_add_item(msg_tree, hf_armagetronad_message_id, tvb,
			    offset + 2, 2, FALSE);

	/
	proto_tree_add_item(msg_tree, hf_armagetronad_data_len, tvb,
			    offset + 4, 2, FALSE);

	/
	add_message_data(tvb, offset + 6, data_len, msg_tree);

	return data_len + 6;
}
