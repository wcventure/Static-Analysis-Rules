static void dissect_rdata_request(tvbuff_t* tvb, proto_tree* lwres_tree)
{
	guint32 rflags;
	guint16 rdclass, rdtype, namelen;
	guint8 name[120];

	proto_item* rdata_request_item;
	proto_tree* rdata_request_tree;

	rflags = tvb_get_ntohl(tvb, LWRES_LWPACKET_LENGTH+0);
	rdclass = tvb_get_ntohs(tvb, LWRES_LWPACKET_LENGTH+4);
	rdtype =  tvb_get_ntohs(tvb, LWRES_LWPACKET_LENGTH+6);
	namelen = tvb_get_ntohs(tvb, LWRES_LWPACKET_LENGTH+8);
	tvb_get_nstringz(tvb, LWRES_LWPACKET_LENGTH+10, namelen, name);
	name[namelen]='\0';

	if(lwres_tree)
	{
		rdata_request_item = 
			proto_tree_add_text(lwres_tree,tvb,LWRES_LWPACKET_LENGTH,10+namelen+1,"RDATA request parameters");
		rdata_request_tree = proto_item_add_subtree(rdata_request_item, ett_rdata_req);
	}
	else 
		return;

	proto_tree_add_uint(rdata_request_tree,
			hf_rflags,
			tvb,
			LWRES_LWPACKET_LENGTH+0,
			sizeof(guint32),
			rflags);

	proto_tree_add_uint(rdata_request_tree,
			hf_rdclass,
			tvb,
			LWRES_LWPACKET_LENGTH+4,
			sizeof(guint16),
			rdclass);

	proto_tree_add_uint(rdata_request_tree,
			hf_rdtype,
			tvb,
			LWRES_LWPACKET_LENGTH+6,
			sizeof(guint16),
			rdtype);

	proto_tree_add_uint(rdata_request_tree,
			hf_namelen,
			tvb,
			LWRES_LWPACKET_LENGTH+8,
			sizeof(guint16),
			namelen);

	proto_tree_add_string(rdata_request_tree,
			hf_req_name,
			tvb,
			LWRES_LWPACKET_LENGTH+10,
			namelen,
		        (char*)name);

}
