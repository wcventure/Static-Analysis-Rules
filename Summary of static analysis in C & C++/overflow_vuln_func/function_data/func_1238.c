static void
dissect_ospf_v2_options (proto_tree *parent_tree, tvbuff_t *tvb, int offset)
{
	proto_item *item=NULL;
	proto_tree *tree=NULL;
	guint8 flags;

	flags = tvb_get_guint8 (tvb, offset);
	if(parent_tree){
		item=proto_tree_add_uint(parent_tree, hf_ospf_options_v2, 
				tvb, offset, 1, flags);
		tree=proto_item_add_subtree(item, ett_ospf_options_v2);
	}

	proto_tree_add_boolean(tree, hf_ospf_options_v2_o, tvb, offset, 1, flags);
	if (flags&OSPF_V2_OPTIONS_O){
		proto_item_append_text(item, "  O");
	}
	flags&=(~( OSPF_V2_OPTIONS_O ));


	proto_tree_add_boolean(tree, hf_ospf_options_v2_dc, tvb, offset, 1, flags);
	if (flags&OSPF_V2_OPTIONS_DC){
		proto_item_append_text(item, "  DC");
	}
	flags&=(~( OSPF_V2_OPTIONS_DC ));

	proto_tree_add_boolean(tree, hf_ospf_options_v2_ea, tvb, offset, 1, flags);
	if (flags&OSPF_V2_OPTIONS_EA){
		proto_item_append_text(item, "  EA");
	}
	flags&=(~( OSPF_V2_OPTIONS_EA ));

	proto_tree_add_boolean(tree, hf_ospf_options_v2_np, tvb, offset, 1, flags);
	if (flags&OSPF_V2_OPTIONS_NP){
		proto_item_append_text(item, "  NP");
	}
	flags&=(~( OSPF_V2_OPTIONS_NP ));

	proto_tree_add_boolean(tree, hf_ospf_options_v2_mc, tvb, offset, 1, flags);
	if (flags&OSPF_V2_OPTIONS_MC){
		proto_item_append_text(item, "  MC");
	}
	flags&=(~( OSPF_V2_OPTIONS_MC ));

	proto_tree_add_boolean(tree, hf_ospf_options_v2_e, tvb, offset, 1, flags);
	if (flags&OSPF_V2_OPTIONS_E){
		proto_item_append_text(item, "  E");
	}
	flags&=(~( OSPF_V2_OPTIONS_E ));

	proto_tree_add_boolean(tree, hf_ospf_options_v2_dn, tvb, offset, 1, flags);
	if (flags&OSPF_V2_OPTIONS_DN){
		proto_item_append_text(item, "  DN");
	}
	flags&=(~( OSPF_V2_OPTIONS_DN ));
}
