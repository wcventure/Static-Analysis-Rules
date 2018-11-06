static void
dissect_eigrp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree) {

  proto_tree *eigrp_tree,*tlv_tree;
  proto_item *ti;
    
  guint opcode,opcode_tmp;
  guint16 tlv,size, offset = EIGRP_HEADER_LENGTH;
  guint32 ack;
      
  if (check_col(pinfo->cinfo, COL_PROTOCOL))
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "EIGRP");
  if (check_col(pinfo->cinfo, COL_INFO))
    col_clear(pinfo->cinfo, COL_INFO);

  opcode_tmp=opcode=tvb_get_guint8(tvb,1);
  ack = tvb_get_ntohl(tvb,12);
  if (opcode==EIGRP_HELLO) { if (ack == 0) opcode_tmp=EIGRP_HI; else opcode_tmp=EIGRP_ACK; }
  
  if (check_col(pinfo->cinfo, COL_INFO))
    col_add_str(pinfo->cinfo, COL_INFO,
	val_to_str(opcode_tmp , eigrp_opcode_vals, "Unknown (0x%04x)"));




  if (tree) {

     ti = proto_tree_add_protocol_format(tree, proto_eigrp, tvb, 0, -1,
              "Cisco EIGRP");
              
     eigrp_tree = proto_item_add_subtree(ti, ett_eigrp);
  
     proto_tree_add_text (eigrp_tree, tvb, 0,1,"Version    = %u",tvb_get_guint8(tvb,0)) ;
     proto_tree_add_uint_format (eigrp_tree, hf_eigrp_opcode, tvb, 1,1,opcode,"Opcode = %u (%s)",opcode,val_to_str(opcode_tmp,eigrp_opcode_vals, "Unknown")) ;
     proto_tree_add_text (eigrp_tree, tvb, 2,2,"Checksum   = 0x%04x",tvb_get_ntohs(tvb,2)) ;
     proto_tree_add_text (eigrp_tree, tvb, 4,4,"Flags      = 0x%08x",tvb_get_ntohl(tvb,4)) ;
     proto_tree_add_text (eigrp_tree, tvb, 8,4,"Sequence   = %u",tvb_get_ntohl(tvb,8)) ;
     proto_tree_add_text (eigrp_tree, tvb, 12,4,"Acknowledge  = %u",tvb_get_ntohl(tvb,12)) ;
     proto_tree_add_uint (eigrp_tree, hf_eigrp_as, tvb, 16,4,tvb_get_ntohl(tvb,16)) ; 

     if (opcode==EIGRP_SAP)
     	{
	call_dissector(ipxsap_handle, tvb_new_subset(tvb, EIGRP_HEADER_LENGTH, -1, -1), pinfo, eigrp_tree);
	return;
	}

     while ( tvb_reported_length_remaining(tvb,offset)>0 ) {

	     tlv = tvb_get_ntohs(tvb,offset);
	     size =  tvb_get_ntohs(tvb,offset+2);
	     if ( size == 0 ) 
		{
		proto_tree_add_text(eigrp_tree,tvb,offset,tvb_length_remaining(tvb,offset),"Unknown data (maybe authentication)");
		return;
		}

	     ti = proto_tree_add_text (eigrp_tree, tvb, offset,size,
	         "%s",val_to_str(tlv, eigrp_tlv_vals, "Unknown (0x%04x)"));

	     tlv_tree = proto_item_add_subtree (ti, ett_tlv);
	     proto_tree_add_uint_format (tlv_tree,hf_eigrp_tlv, tvb,offset,2,tlv,"Type = 0x%04x (%s)",tlv,val_to_str(tlv,eigrp_tlv_vals, "Unknown")) ;
	     proto_tree_add_text (tlv_tree,tvb,offset+2,2,"Size = %u bytes",size) ;

                     
	     switch (tlv){
	     	case TLV_PAR:
	     		dissect_eigrp_par(tvb_new_subset(tvb, offset+4, size-4, -1), tlv_tree, ti);
	     		break;
	     	case TLV_SEQ:
	     		dissect_eigrp_seq(tvb_new_subset(tvb, offset+4, size-4, -1), tlv_tree, ti);
	     		break;
	     	case TLV_SV:
	     		dissect_eigrp_sv(tvb_new_subset(tvb, offset+4, size-4, -1), tlv_tree, ti);
     			break;
	     	case TLV_NMS:
     			dissect_eigrp_nms(tvb_new_subset(tvb, offset+4, size-4, -1), tlv_tree, ti);
     			break;

	     	case TLV_IP_INT:
     			dissect_eigrp_ip_int(tvb_new_subset(tvb, offset+4, size-4, -1), tlv_tree, ti);
     			break;                   
	     	case TLV_IP_EXT:
     			dissect_eigrp_ip_ext(tvb_new_subset(tvb, offset+4, size-4, -1), tlv_tree, ti);
     			break;                   

	     	case TLV_IPX_INT:
     			dissect_eigrp_ipx_int(tvb_new_subset(tvb, offset+4, size-4, -1), tlv_tree, ti);
     			break;                   
	     	case TLV_IPX_EXT:
     			dissect_eigrp_ipx_ext(tvb_new_subset(tvb, offset+4, size-4, -1), tlv_tree, ti);
     			break;                   
	
	     	case TLV_AT_CBL:
     			dissect_eigrp_at_cbl(tvb_new_subset(tvb, offset+4, size-4, -1), tlv_tree, ti);
     			break;                   
	     	case TLV_AT_INT:
     			dissect_eigrp_at_int(tvb_new_subset(tvb, offset+4, size-4, -1), tlv_tree, ti);
     			break;                   
	     	case TLV_AT_EXT:
     			dissect_eigrp_at_ext(tvb_new_subset(tvb, offset+4, size-4, -1), tlv_tree, ti);
     			break;                   
		case TLV_AUTH:
			proto_tree_add_text(tlv_tree,tvb,offset+4,size-4,"Authentication data");
			break;
	     	};

	     offset+=size;
     }     

   }
}
