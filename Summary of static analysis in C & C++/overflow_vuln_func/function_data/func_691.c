static void dissect_getaddrsbyname_response(tvbuff_t* tvb, proto_tree* lwres_tree)
{
	guint32 flags, family ,i, offset;
	guint16 naliases, naddrs, realnamelen, length, aliaslen;
	const gchar* addr;
	guint slen;
	char realname[120];
	char aliasname[120];

	proto_item* adn_resp_item;
	proto_tree* adn_resp_tree;
	proto_item* alias_item;
	proto_tree* alias_tree;
	proto_item* addr_item;
	proto_tree* addr_tree;

	

	if(lwres_tree)
	{
		adn_resp_item = proto_tree_add_text(lwres_tree, tvb, LWRES_LWPACKET_LENGTH, 10, "getaddrbyname records");
		adn_resp_tree = proto_item_add_subtree(adn_resp_item, ett_adn_resp);
	}
	else return;

	flags = tvb_get_ntohl(tvb, LWRES_LWPACKET_LENGTH);
	naliases = tvb_get_ntohs(tvb, LWRES_LWPACKET_LENGTH + 4);
	naddrs   = tvb_get_ntohs(tvb, LWRES_LWPACKET_LENGTH + 6);
	realnamelen = tvb_get_ntohs(tvb, LWRES_LWPACKET_LENGTH + 8);
	tvb_get_nstringz(tvb, LWRES_LWPACKET_LENGTH + 10, realnamelen, (guint8*)realname);
	realname[realnamelen]='\0';

	
	proto_tree_add_uint(adn_resp_tree,
						hf_adn_flags,
						tvb, 
						LWRES_LWPACKET_LENGTH,
						4,
						flags);

	proto_tree_add_uint(adn_resp_tree,
						hf_adn_naliases,
						tvb, 
						LWRES_LWPACKET_LENGTH + 4,
						2,
						naliases);

	proto_tree_add_uint(adn_resp_tree,
						hf_adn_naddrs,
						tvb,
						LWRES_LWPACKET_LENGTH + 6,
						2,
						naddrs);

	proto_tree_add_uint(adn_resp_tree,
						hf_adn_namelen,
						tvb,
						LWRES_LWPACKET_LENGTH + 8,
						2, 
						realnamelen);
	
	proto_tree_add_string(adn_resp_tree,
						hf_adn_realname,
						tvb,
						LWRES_LWPACKET_LENGTH + 10,
						realnamelen,
						realname);

	offset = LWRES_LWPACKET_LENGTH + 10 + realnamelen + 1;

	if(naliases)
	{
		for(i=0; i<naliases; i++)
		{
			aliaslen = tvb_get_ntohs(tvb, offset);
			tvb_get_nstringz(tvb, offset + 2, aliaslen, (guint8*)aliasname);
			aliasname[aliaslen]='\0';

			alias_item = proto_tree_add_text(adn_resp_tree, tvb, offset, 2 + aliaslen, "Alias %s",aliasname);
			alias_tree = proto_item_add_subtree(alias_item, ett_adn_alias);

			proto_tree_add_uint(alias_tree,
								hf_adn_namelen,
								tvb,
								offset,
								2,
								aliaslen);

			proto_tree_add_string(alias_tree,
								hf_adn_aliasname,
								tvb,
								offset + 2,
								aliaslen,
								aliasname);

			offset+=(2 + aliaslen + 1);
		}
	}

	if(naddrs)
	{
		for(i=0; i < naddrs; i++)
		{
			family = tvb_get_ntohl(tvb, offset);
			length = tvb_get_ntohs(tvb, offset + 4);
			addr = (gchar*)tvb_get_ptr(tvb, offset + 6, 4);
			slen = (int)strlen((char*)ip_to_str((guint8*)addr));
		
			addr_item = proto_tree_add_text(adn_resp_tree,tvb, offset, 4+2+4, "Address %s",ip_to_str((guint8*)addr));
			addr_tree = proto_item_add_subtree(addr_item, ett_adn_addr);

			proto_tree_add_uint(addr_tree, 
								hf_adn_family,
								tvb, 
								offset, 
								4,
								family);

			proto_tree_add_uint(addr_tree,
								hf_adn_addr_len,
								tvb,
								offset + 4,
								2,
								length);

			proto_tree_add_string(addr_tree,
								hf_adn_addr_addr,
								tvb,
								offset + 6,
								slen,
					                        ip_to_str((guint8*)addr));

			offset+= 4 + 2 + 4;
		}
	}


}
