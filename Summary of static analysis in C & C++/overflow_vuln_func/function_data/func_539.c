static void dissect_eigrp_ip_int (tvbuff_t *tvb, proto_tree *tree, proto_item *ti)
{
	guint8 ip_addr[4],length,addr_len;
	tvb_memcpy(tvb,ip_addr,0,4);
	proto_tree_add_text (tree,tvb,0,4, "Next Hop    = %s",ip_to_str(ip_addr));
	proto_tree_add_text (tree,tvb,4,4, "Delay       = %u",tvb_get_ntohl(tvb,4));
	proto_tree_add_text (tree,tvb,8,4, "Bandwidth   = %u",tvb_get_ntohl(tvb,8));
	proto_tree_add_text (tree,tvb,12,3,"MTU         = %u",tvb_get_ntoh24(tvb,12));
	proto_tree_add_text (tree,tvb,15,1,"Hop Count   = %u",tvb_get_guint8(tvb,15));
	proto_tree_add_text (tree,tvb,16,1,"Reliability = %u",tvb_get_guint8(tvb,16));
	proto_tree_add_text (tree,tvb,17,1,"Load        = %u",tvb_get_guint8(tvb,17));
	proto_tree_add_text (tree,tvb,18,2,"Reserved ");
	length=tvb_get_guint8(tvb,20);
	proto_tree_add_text (tree,tvb,20,1,"Prefix Length = %u",length);
	if (length % 8 == 0) addr_len=length/8 ; else addr_len=length/8+1;
	ip_addr[0]=ip_addr[1]=ip_addr[2]=ip_addr[3]=0;
	tvb_memcpy(tvb,ip_addr,21,addr_len);
        proto_tree_add_text (tree,tvb,21,addr_len,"Destination = %s",ip_to_str(ip_addr));
        proto_item_append_text (ti,"  =   %s/%u%s",ip_to_str(ip_addr),length,((tvb_get_ntohl(tvb,4)==0xffffffff)?" - Destination unreachable":""));
}
