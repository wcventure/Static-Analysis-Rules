static void
dissect_rsvp_hello (proto_tree *ti, proto_tree *rsvp_object_tree,
		    tvbuff_t *tvb,
		    int offset, int obj_length _U_,
		    int class _U_, int type)
{
    switch(type) {
    case 1:
    case 2:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-Type: 1 - HELLO %s object",
			    type==1 ? "REQUEST" : "ACK");
	proto_tree_add_text(rsvp_object_tree, tvb, offset+4, 4,
			    "Source Instance: 0x%x",tvb_get_ntohl(tvb, offset+4));
	proto_tree_add_text(rsvp_object_tree, tvb, offset+8, 4,
			    "Destination Instance: 0x%x",tvb_get_ntohl(tvb, offset+8));
	proto_item_append_text(ti, ": %s. Src Instance: 0x%0x. Dest Instance: 0x%0x. ", 
			       type==1 ? "REQUEST" : "ACK", 
			       tvb_get_ntohl(tvb, offset+4),
			       tvb_get_ntohl(tvb, offset+8));
	break;
    default:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-Type: %d - UNKNOWN", type);
	break;
    };
}
