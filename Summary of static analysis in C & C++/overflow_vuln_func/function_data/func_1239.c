void
proto_register_ospf(void)
{
    static gint *ett[] = {
	&ett_ospf,
	&ett_ospf_hdr,
	&ett_ospf_hello,
	&ett_ospf_desc,
	&ett_ospf_lsr,
	&ett_ospf_lsa,
        &ett_ospf_lsa_router_link,
	&ett_ospf_lsa_upd,
	&ett_ospf_lsa_mpls,
	&ett_ospf_lsa_mpls_router,
	&ett_ospf_lsa_mpls_link,
	&ett_ospf_lsa_mpls_link_stlv,
	&ett_ospf_lsa_mpls_link_stlv_admingrp,
        &ett_ospf_lsa_oif_tna,
        &ett_ospf_lsa_oif_tna_stlv,
        &ett_ospf_options_v2,
        &ett_ospf_options_v3,
        &ett_ospf_dbd
    };

    proto_ospf = proto_register_protocol("Open Shortest Path First",
					 "OSPF", "ospf");
    proto_register_field_array(proto_ospf, ospff_info, array_length(ospff_info));
    proto_register_subtree_array(ett, array_length(ett));
}
