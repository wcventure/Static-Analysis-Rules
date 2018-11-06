static guint
vnc_server_set_colormap_entries(tvbuff_t *tvb, packet_info *pinfo, gint *offset,
				proto_tree *tree)
{
	guint16 number_of_colors;
	guint counter, bytes_needed;
	proto_item *ti;
	proto_tree *vnc_colormap_num_groups, *vnc_colormap_color_group;

	col_set_str(pinfo->cinfo, COL_INFO, "Server set colormap entries");

	number_of_colors = tvb_get_ntohs(tvb, 4);

	bytes_needed = (number_of_colors * 6) + 6;
	VNC_BYTES_NEEDED(bytes_needed);

	ti = proto_tree_add_item(tree, hf_vnc_server_message_type, tvb,
				 *offset, 1, FALSE);
	tree = proto_item_add_subtree(ti, ett_vnc_server_message_type);
	*offset += 1;

	proto_tree_add_item(tree, hf_vnc_padding, tvb, *offset, 1, FALSE);
	*offset += 1; /

	proto_tree_add_item(tree,
			    hf_vnc_colormap_first_color,
			    tvb, *offset, 2, FALSE);
	*offset += 2;

	ti = proto_tree_add_item(tree, hf_vnc_colormap_num_colors, tvb,
				 *offset, 2, FALSE);
	vnc_colormap_num_groups =
		proto_item_add_subtree(ti, ett_vnc_colormap_num_groups);

	*offset += 2;

	for(counter = 1; counter <= number_of_colors; counter++) {
		ti = proto_tree_add_text(vnc_colormap_num_groups, tvb,
					 *offset, 6,
					 "Color group #%d", counter);

		vnc_colormap_color_group =
			proto_item_add_subtree(ti,
					       ett_vnc_colormap_color_group);

		proto_tree_add_item(vnc_colormap_color_group,
				    hf_vnc_colormap_red, tvb,
				    *offset, 2, FALSE);
		*offset += 2;

		proto_tree_add_item(vnc_colormap_color_group,
				    hf_vnc_colormap_green, tvb,
				    *offset, 2, FALSE);
		*offset += 2;

		proto_tree_add_item(vnc_colormap_color_group,
				    hf_vnc_colormap_blue, tvb,
				    *offset, 2, FALSE);
		*offset += 2;
	}
	return *offset;
}
