static void dissect_getnamebyaddr_response(tvbuff_t* tvb, proto_tree* lwres_tree)
{
	guint32 flags,i, offset;
	guint16 naliases,realnamelen,aliaslen;
	char aliasname[120];
	char realname[120];
	

	proto_item* nba_resp_item;
	proto_tree* nba_resp_tree;

	proto_item* alias_item;
	proto_tree* alias_tree;

	if(lwres_tree)
	{
		nba_resp_item = proto_tree_add_text(lwres_tree, tvb, LWRES_LWPACKET_LENGTH, 10,"getnamebyaddr records");
		nba_resp_tree = proto_item_add_subtree(nba_resp_item, ett_nba_resp);
	}
	else return;

	flags = tvb_get_ntohl(tvb, LWRES_LWPACKET_LENGTH);
	naliases = tvb_get_ntohs(tvb, LWRES_LWPACKET_LENGTH + 4);
	realnamelen = tvb_get_ntohs(tvb,LWRES_LWPACKET_LENGTH + 4 + 2);
	tvb_get_nstringz(tvb, LWRES_LWPACKET_LENGTH + 4 + 2 + 2, realnamelen, (guint8*)realname);
	realname[realnamelen]='\0';

	proto_tree_add_uint(nba_resp_tree,
						hf_adn_flags,
						tvb,
						LWRES_LWPACKET_LENGTH,
						4,
						flags);
	proto_tree_add_uint(nba_resp_tree,
						hf_adn_naliases,
						tvb,
						LWRES_LWPACKET_LENGTH + 4,
						2,
						naliases);

	proto_tree_add_uint(nba_resp_tree,
						hf_adn_namelen,
						tvb,
						LWRES_LWPACKET_LENGTH + 6,
						2, 
						realnamelen);

	proto_tree_add_string(nba_resp_tree,
						  hf_adn_realname,
						  tvb,
						  LWRES_LWPACKET_LENGTH + 8,
						  realnamelen,
						  realname);

	offset=LWRES_LWPACKET_LENGTH + 8 + realnamelen;

	if(naliases)
	{
		for(i=0; i<naliases; i++)
		{
			aliaslen = tvb_get_ntohs(tvb, offset);
			tvb_get_nstringz(tvb, offset + 2, aliaslen, (guint8*)aliasname);
			aliasname[aliaslen]='\0';

			alias_item = proto_tree_add_text(nba_resp_tree, tvb, offset, 2 + aliaslen, "Alias %s",aliasname);
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
}
