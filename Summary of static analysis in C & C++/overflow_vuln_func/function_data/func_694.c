static void dissect_ns_records(tvbuff_t* tvb, proto_tree* tree, guint32 nrec, int offset)
{
	guint32 i, curr;
	guint16 len, dlen, namelen;
	guint8 dname[120];

	proto_item* ns_rec_item, *rec_item;
	proto_tree* ns_rec_tree, *rec_tree;
	
	if(tree)
	{
		ns_rec_item = proto_tree_add_text(tree, tvb, offset, offset, "NS record (%d)", nrec);
		ns_rec_tree = proto_item_add_subtree(ns_rec_item, ett_ns_rec);
	}
	else
		return;
	curr=offset;

	for(i=0;i<nrec;i++)
	{
		len = tvb_get_ntohs(tvb, curr);
		namelen = len - 2;
		dlen = lwres_get_dns_name(tvb, curr + 2, curr, (char*)dname, sizeof(dname));
		if(ns_rec_tree)
		{
			rec_item = proto_tree_add_text(ns_rec_tree, tvb, curr,4, "NS record: dname=%s",dname);
			rec_tree = proto_item_add_subtree(rec_item, ett_ns_rec_item);
		}
		else
			return;

		proto_tree_add_text(rec_tree,
							tvb,
							curr + 2,
							dlen,
							"Name: %s", dname);
		curr+=(sizeof(short) + dlen);
							
	}
	

}
