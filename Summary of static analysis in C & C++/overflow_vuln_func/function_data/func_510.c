void
proto_register_rsvp(void)
{
    gint i;

    /
    for (i=0; i<TT_MAX; i++)
	ett_tree[i] = &(ett_treelist[i]);

    proto_rsvp = proto_register_protocol("Resource ReserVation Protocol (RSVP)",
					 "RSVP", "rsvp");
    proto_register_field_array(proto_rsvp, rsvpf_info, array_length(rsvpf_info));
    proto_register_subtree_array(ett_tree, array_length(ett_tree));
    register_rsvp_prefs();

    rsvp_dissector_table = register_dissector_table("rsvp.proto", "RSVP Protocol",
						    FT_UINT8, BASE_DEC);
						    
    /
    register_init_routine(&rsvp_init_protocol);
}
