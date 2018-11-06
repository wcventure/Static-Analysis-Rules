static void dissect_rdata_response(tvbuff_t* tvb, proto_tree* lwres_tree)
{
	guint32 rflags, ttl, offset;
	guint16 rdclass, rdtype, nrdatas, nsigs, realnamelen;
	guint8 realname[120];

	proto_item* rdata_resp_item;
	proto_tree* rdata_resp_tree;

	rflags = tvb_get_ntohl(tvb, LWRES_LWPACKET_LENGTH+0);
	rdclass = tvb_get_ntohs(tvb, LWRES_LWPACKET_LENGTH+4);
	rdtype =  tvb_get_ntohs(tvb, LWRES_LWPACKET_LENGTH+6);
	ttl    =  tvb_get_ntohl(tvb, LWRES_LWPACKET_LENGTH+8);
	nrdatas = tvb_get_ntohs(tvb, LWRES_LWPACKET_LENGTH+12);
	nsigs   = tvb_get_ntohs(tvb, LWRES_LWPACKET_LENGTH+14);
	realnamelen = tvb_get_ntohs(tvb,LWRES_LWPACKET_LENGTH+16);
	tvb_get_nstringz(tvb,LWRES_LWPACKET_LENGTH+18,realnamelen, realname);
	realname[realnamelen]='\0';

	offset = LWRES_LWPACKET_LENGTH + 18 + realnamelen + 1;

	if(lwres_tree)
	{
		rdata_resp_item = proto_tree_add_text(lwres_tree,tvb,LWRES_LWPACKET_LENGTH, 18+realnamelen+1,"RDATA response");
		rdata_resp_tree = proto_item_add_subtree(rdata_resp_item, ett_rdata_resp);
	}
	else
		return;

	proto_tree_add_uint(rdata_resp_tree,
                        hf_rflags,
                        tvb,
                        LWRES_LWPACKET_LENGTH+0,
                        sizeof(guint32),
                        rflags);

	proto_tree_add_uint(rdata_resp_tree,
                        hf_rdclass,
                        tvb,
                        LWRES_LWPACKET_LENGTH+4,
                        sizeof(guint16),
                        rdclass);

	proto_tree_add_uint(rdata_resp_tree,
                        hf_rdtype,
                        tvb,
                        LWRES_LWPACKET_LENGTH+6,
                        sizeof(guint16),
                        rdtype);

	proto_tree_add_uint(rdata_resp_tree,
			hf_ttl,
			tvb,
			LWRES_LWPACKET_LENGTH+8,
			sizeof(guint32),
			ttl);

	proto_tree_add_uint(rdata_resp_tree,
			hf_nrdatas,
			tvb,
			LWRES_LWPACKET_LENGTH+12,
			sizeof(guint16),
			nrdatas);

	proto_tree_add_uint(rdata_resp_tree,
			hf_nsigs,
			tvb,
			LWRES_LWPACKET_LENGTH+14,
			sizeof(guint16),
			nsigs);

	proto_tree_add_uint(rdata_resp_tree,
			hf_realnamelen,
			tvb,
			LWRES_LWPACKET_LENGTH+16,
			sizeof(guint16),
			realnamelen);

	proto_tree_add_string(rdata_resp_tree,
                        hf_realname,
                        tvb,
                        LWRES_LWPACKET_LENGTH+18,
                        realnamelen,
		        (char*)realname);

	switch(rdtype)
	{
		case T_A:
			dissect_a_records(tvb,rdata_resp_tree,nrdatas,offset);
		break;

		case T_SRV:
			dissect_srv_records(tvb,rdata_resp_tree,nrdatas, offset);
		break;

		case T_MX:
			dissect_mx_records(tvb,rdata_resp_tree,nrdatas, offset);
		break;

		case T_NS:
			dissect_ns_records(tvb,rdata_resp_tree,nrdatas, offset);
		break;
	}

}
