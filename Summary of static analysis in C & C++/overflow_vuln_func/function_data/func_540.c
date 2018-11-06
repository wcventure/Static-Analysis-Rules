static void dissect_eigrp_ip_ext (tvbuff_t *tvb, proto_tree *tree, proto_item *ti)
{
	guint8 ip_addr[4],length,addr_len;
	tvb_memcpy(tvb,ip_addr,0,4);
	proto_tree_add_text (tree,tvb,0,4,"Next Hop = %s",ip_to_str(ip_addr));
	tvb_memcpy(tvb,ip_addr,4,4);
	proto_tree_add_text (tree,tvb,4,4,"Originating router = %s",ip_to_str(ip_addr));
	proto_tree_add_text (tree,tvb,8,4,"Originating A.S. = %u",tvb_get_ntohl(tvb,8));
	proto_tree_add_text (tree,tvb,12,4,"Arbitrary tag = %u",tvb_get_ntohl(tvb,12));
	proto_tree_add_text (tree,tvb,16,4,"External protocol metric = %u",tvb_get_ntohl(tvb,16));
	proto_tree_add_text (tree,tvb,20,2,"Reserved");
	proto_tree_add_text (tree,tvb,22,1,"External protocol ID = %u (%s)",tvb_get_guint8(tvb,22),val_to_str(tvb_get_guint8(tvb,22),eigrp_pid_vals, "Unknown"));
	proto_tree_add_text (tree,tvb,23,1,"Flags = 0x%0x",tvb_get_guint8(tvb,23));

	proto_tree_add_text (tree,tvb,24,4,"Delay     = %u",tvb_get_ntohl(tvb,24));
	proto_tree_add_text (tree,tvb,28,4,"Bandwidth = %u",tvb_get_ntohl(tvb,28));
	proto_tree_add_text (tree,tvb,32,3,"MTU    = %u",tvb_get_ntoh24(tvb,32));
	proto_tree_add_text (tree,tvb,35,1,"Hop Count = %u",tvb_get_guint8(tvb,35));
	proto_tree_add_text (tree,tvb,36,1,"Reliability = %u",tvb_get_guint8(tvb,36));
	proto_tree_add_text (tree,tvb,37,1,"Load = %u",tvb_get_guint8(tvb,37));
	proto_tree_add_text (tree,tvb,38,2,"Reserved ");
	length=tvb_get_guint8(tvb,40);
	proto_tree_add_text (tree,tvb,40,1,"Prefix Length = %u",length);
        if (length % 8 == 0) addr_len=length/8 ; else addr_len=length/8+1;
        ip_addr[0]=ip_addr[1]=ip_addr[2]=ip_addr[3]=0;
        tvb_memcpy(tvb,ip_addr,41,addr_len);
        proto_tree_add_text (tree,tvb,41,addr_len,"Destination = %s",ip_to_str(ip_addr));
        proto_item_append_text (ti,"  =   %s/%u%s",ip_to_str(ip_addr),length,((tvb_get_ntohl(tvb,24)==0xffffffff)?" - Destination unreachable":""));
}
