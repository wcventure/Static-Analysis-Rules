static void
dissect_sap(const u_char *pd, int offset, frame_data *fd, proto_tree *tree) {

	proto_tree	*sap_tree, *s_tree;
	proto_item	*ti;
	int		cursor;
	struct sap_query query;
	struct sap_server_ident server;

	char		*sap_type[4] = { "General Query", "General Response",
		"Nearest Query", "Nearest Response" };

	query.query_type = pntohs(&pd[offset]);
	query.server_type = pntohs(&pd[offset+2]);

	if (check_col(fd, COL_PROTOCOL))
		col_add_str(fd, COL_PROTOCOL, "SAP");
	if (check_col(fd, COL_INFO)) {
		if (query.query_type < 4) {
			col_add_str(fd, COL_INFO, sap_type[query.query_type - 1]);
		}
		else {
			col_add_str(fd, COL_INFO, "Unknown Packet Type");
		}
	}

	if (tree) {
		ti = proto_tree_add_item(tree, proto_sap, offset, END_OF_FRAME, NULL);
		sap_tree = proto_item_add_subtree(ti, ETT_IPXSAP);

		if (query.query_type < 4) {
			proto_tree_add_text(sap_tree, offset, 2, sap_type[query.query_type - 1]);
		}
		else {
			proto_tree_add_text(sap_tree, offset, 2,
					"Unknown SAP Packet Type %d", query.query_type);
		}

		if (query.query_type == IPX_SAP_GENERAL_RESPONSE ||
				query.query_type == IPX_SAP_NEAREST_RESPONSE) { /

			for (cursor = offset + 2; (cursor + 64) <= fd->cap_len; cursor += 64) {
				server.server_type = pntohs(&pd[cursor]);
				memcpy(server.server_name, &pd[cursor+2], 48);
				memcpy(&server.server_network, &pd[cursor+50], 4);
				memcpy(&server.server_node, &pd[cursor+54], 6);
				server.server_port = pntohs(&pd[cursor+60]);
				server.intermediate_network = pntohs(&pd[cursor+62]);

				ti = proto_tree_add_text(sap_tree, cursor+2, 48,
					"Server Name: %s", server.server_name);
				s_tree = proto_item_add_subtree(ti, ETT_IPXSAP_SERVER);

				proto_tree_add_text(s_tree, cursor, 2, "Server Type: %s (0x%04X)",
						server_type(server.server_type), server.server_type);
				proto_tree_add_text(s_tree, cursor+50, 4, "Network: %s",
						ipxnet_to_string((guint8*)&pd[cursor+50]));
				proto_tree_add_text(s_tree, cursor+54, 6, "Node: %s",
						ether_to_str((guint8*)&pd[cursor+54]));
				proto_tree_add_text(s_tree, cursor+60, 2, "Socket: %s (0x%04X)",
						port_text(server.server_port), server.server_port);
				proto_tree_add_text(s_tree, cursor+62, 2,
						"Intermediate Networks: %d",
						server.intermediate_network);
			}
		}
		else {  /
			proto_tree_add_text(sap_tree, offset+2, 2, "Server Type: %s (0x%04X)",
					server_type(query.server_type), query.server_type);
		}
	}
}
