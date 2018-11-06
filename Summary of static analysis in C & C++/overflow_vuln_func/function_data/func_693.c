static void dissect_mx_records(tvbuff_t* tvb, proto_tree* tree, guint32 nrec, int offset)
{
	
	guint32 i, curr;
	guint16 len, priority, dlen, namelen;
	const char* cname;
	guint8 dname[120];

	proto_item* mx_rec_item, *rec_item;
	proto_tree* mx_rec_tree, *rec_tree;
	

	if(tree)
	{
		mx_rec_item = proto_tree_add_text(tree, tvb, offset, offset, "MX records (%d)", nrec);
		mx_rec_tree = proto_item_add_subtree(mx_rec_item, ett_mx_rec);
	}
	else
		return;
	
	curr = offset;
	for(i=0; i < nrec; i++)
	{
		len =		tvb_get_ntohs(tvb, curr);
		priority =  tvb_get_ntohs(tvb, curr + 2);
		namelen  =  len - 4;
		cname = (char*)tvb_get_ptr(tvb, curr + 4, 4);
		dlen  = lwres_get_dns_name(tvb, curr + 4, curr, (gchar*)dname, sizeof(dname));
		if(mx_rec_tree)
		{
			rec_item = proto_tree_add_text(mx_rec_tree, tvb, curr,6,"MX record: pri=%d,dname=%s",
						priority,dname);
			rec_tree = proto_item_add_subtree(rec_item, ett_mx_rec_item);
		}
		else 
			return;

		
		proto_tree_add_uint(rec_tree,
							hf_srv_prio,
							tvb,
							curr + 2,
							2,
							priority);
	
		proto_tree_add_text(rec_tree,
							tvb,
							curr + 4,
							dlen,
							"name: %s", dname);

		curr+=((sizeof(short)*2) + dlen);
	

	}
	
}
