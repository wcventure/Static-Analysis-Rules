static void dissect_srv_records(tvbuff_t* tvb, proto_tree* tree,guint32 nrec,int offset)
{
	guint32 i, curr;
	guint16 len, priority, weight, port, namelen, dlen;
	const char *cmpname;
	guint8 dname[120];

	proto_item* srv_rec_item, *rec_item;
	proto_item* srv_rec_tree, *rec_tree;

	if(tree)
	{
		srv_rec_item = proto_tree_add_text(tree, tvb, offset, offset, "SRV records");
		srv_rec_tree = proto_item_add_subtree(srv_rec_item, ett_srv_rec);
			       proto_item_set_text(srv_rec_item, "SRV records (%d)", nrec);
	}
	else return;

	curr = offset;

	for(i=0; i < nrec; i++)
	{
		len =      tvb_get_ntohs(tvb, curr);
		priority = tvb_get_ntohs(tvb, curr + 2);
		weight   = tvb_get_ntohs(tvb, curr + 4);
		port     = tvb_get_ntohs(tvb, curr + 6);
		namelen = len - 8;
		cmpname  = (char*)tvb_get_ptr(tvb, curr + 8, namelen);

		dlen = lwres_get_dns_name(tvb, curr + 8, curr, (gchar*)dname, sizeof(dname));

		if(srv_rec_tree)
		{
			rec_item = proto_tree_add_text(srv_rec_tree, tvb, curr, 6,"  ");
			rec_tree = proto_item_add_subtree(rec_item, ett_srv_rec_item);
			proto_item_set_text(rec_item,
						"SRV record:pri=%d,w=%d,port=%d,dname=%s",
						priority,
						weight,
						port,
						dname); 
		}
		else return;

		proto_tree_add_uint(rec_tree,
						hf_srv_prio,
						tvb,
						curr + 2,
						2,
						priority);

		proto_tree_add_uint(rec_tree,
						hf_srv_weight,
						tvb,
						curr + 4,
						2,
						weight);

		proto_tree_add_uint(rec_tree,
						hf_srv_port,
						tvb,
						curr + 6,
						2,
						port);


		proto_tree_add_text(rec_tree,
							tvb,
							curr + 8,
							dlen,
							"DNAME: %s", dname);

		curr+=((sizeof(short)*4) + dlen);
							
	}

}
