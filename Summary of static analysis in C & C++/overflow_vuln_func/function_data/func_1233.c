static void
dissect_ospf_hello(tvbuff_t *tvb, int offset, proto_tree *tree, guint8 version)
{
    proto_tree *ospf_hello_tree;
    proto_item *ti;

    ti = proto_tree_add_text(tree, tvb, offset, -1, "OSPF Hello Packet");
    ospf_hello_tree = proto_item_add_subtree(ti, ett_ospf_hello);

    switch (version ) {
        case OSPF_VERSION_2:
            proto_tree_add_text(ospf_hello_tree, tvb, offset, 4, "Network Mask: %s",
			ip_to_str(tvb_get_ptr(tvb, offset, 4)));
            proto_tree_add_text(ospf_hello_tree, tvb, offset + 4, 2,
			"Hello Interval: %u seconds",
			tvb_get_ntohs(tvb, offset + 4));

            dissect_ospf_options(tvb, offset + 6, ospf_hello_tree, version);
            proto_tree_add_text(ospf_hello_tree, tvb, offset + 7, 1, "Router Priority: %u",
			tvb_get_guint8(tvb, offset + 7));
            proto_tree_add_text(ospf_hello_tree, tvb, offset + 8, 4, "Router Dead Interval: %u seconds",
			tvb_get_ntohl(tvb, offset + 8));
            proto_tree_add_text(ospf_hello_tree, tvb, offset + 12, 4, "Designated Router: %s",
			ip_to_str(tvb_get_ptr(tvb, offset + 12, 4)));
            proto_tree_add_text(ospf_hello_tree, tvb, offset + 16, 4, "Backup Designated Router: %s",
			ip_to_str(tvb_get_ptr(tvb, offset + 16, 4)));

            offset += 20;
            while (tvb_reported_length_remaining(tvb, offset) != 0) {
	        proto_tree_add_text(ospf_hello_tree, tvb, offset, 4,
			    "Active Neighbor: %s",
			    ip_to_str(tvb_get_ptr(tvb, offset, 4)));
	        offset += 4;
            }
            break;
        case OSPF_VERSION_3:
            proto_tree_add_text(ospf_hello_tree, tvb, offset + 0, 4, "Interface ID: %u",
			tvb_get_ntohl(tvb, offset + 0));
            proto_tree_add_text(ospf_hello_tree, tvb, offset + 4, 1, "Router Priority: %u",
			tvb_get_guint8(tvb, offset + 4));
            dissect_ospf_options(tvb, offset + 5, ospf_hello_tree, version);
            proto_tree_add_text(ospf_hello_tree, tvb, offset + 8, 2,
			"Hello Interval: %u seconds",
			tvb_get_ntohs(tvb, offset + 8));
            proto_tree_add_text(ospf_hello_tree, tvb, offset + 10, 2, "Router Dead Interval: %u seconds",
			tvb_get_ntohs(tvb, offset + 10));
            proto_tree_add_text(ospf_hello_tree, tvb, offset + 12, 4, "Designated Router: %s",
			ip_to_str(tvb_get_ptr(tvb, offset + 12, 4)));
            proto_tree_add_text(ospf_hello_tree, tvb, offset + 16, 4, "Backup Designated Router: %s",
			ip_to_str(tvb_get_ptr(tvb, offset + 16, 4)));
            offset += 20;
            while (tvb_reported_length_remaining(tvb, offset) != 0) {
	        proto_tree_add_text(ospf_hello_tree, tvb, offset, 4,
			    "Active Neighbor: %s",
			    ip_to_str(tvb_get_ptr(tvb, offset, 4)));
	        offset += 4;
            }

	    break;
    }
}
