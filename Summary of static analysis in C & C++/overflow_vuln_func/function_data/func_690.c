static void dissect_getaddrsbyname_request(tvbuff_t* tvb, proto_tree* lwres_tree)
{
	guint32 flags,addrtype;
	guint16 namelen;
	guint8  name[120];

	proto_item* adn_request_item;
	proto_tree* adn_request_tree;
	
	flags = tvb_get_ntohl(tvb, LWRES_LWPACKET_LENGTH);
	addrtype = tvb_get_ntohl(tvb, LWRES_LWPACKET_LENGTH + 4);
	namelen  = tvb_get_ntohs(tvb, LWRES_LWPACKET_LENGTH + 8);
	tvb_get_nstringz(tvb, LWRES_LWPACKET_LENGTH+10, namelen, name);
        name[namelen]='\0';

	if(lwres_tree)
	{
		adn_request_item = proto_tree_add_text(lwres_tree,tvb,
						LWRES_LWPACKET_LENGTH,10+namelen+1,
						"getaddrbyname parameters");
		adn_request_tree = proto_item_add_subtree(adn_request_item, ett_adn_request);
	}
	else
		return;


	proto_tree_add_uint(adn_request_tree,
				hf_adn_flags,
				tvb,
				LWRES_LWPACKET_LENGTH+0,
				sizeof(guint32),
				flags);

	proto_tree_add_uint(adn_request_tree,
				hf_adn_addrtype,
				tvb,
				LWRES_LWPACKET_LENGTH+4,
				sizeof(guint32),
				addrtype);

	proto_tree_add_uint(adn_request_tree,
				hf_adn_namelen,
				tvb,
				LWRES_LWPACKET_LENGTH+8,
				sizeof(guint16),
				namelen);

	proto_tree_add_string(adn_request_tree,
				hf_adn_name,
				tvb,
				LWRES_LWPACKET_LENGTH+10,
				namelen,
			        (gchar*)name);
	
}
