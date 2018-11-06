void
dissect_nsap(tvbuff_t *parameter_tvb,gint offset,gint len, proto_tree *parameter_tree)
{
	guint8 afi, cc_length = 0;
	guint8 length = 0, address_digit_pair = 0;
	guint icp,  cc, id_code, cc_offset;

	afi = tvb_get_guint8(parameter_tvb, offset);

	switch ( afi ) {
		case 0x35: 	/
			proto_tree_add_text(parameter_tree, parameter_tvb, offset, 3,
			    "IDP = %s", tvb_bytes_to_str(parameter_tvb, offset, 3));

			proto_tree_add_uint(parameter_tree, hf_afi, parameter_tvb, offset, 1, afi );
			offset = offset + 1;
			icp = tvb_get_ntohs(parameter_tvb, offset);
			proto_tree_add_uint(parameter_tree, hf_iana_icp, parameter_tvb, offset, 1, icp );
			if ( icp == 0 ){ /
				proto_tree_add_text(parameter_tree, parameter_tvb, offset + 2 , 17,
				    "DSP = %s", tvb_bytes_to_str(parameter_tvb, offset + 2, 17));
				proto_tree_add_item(parameter_tree, hf_nsap_ipv6_addr, parameter_tvb, offset + 2,
				    16, FALSE);

			}
			else { /
				/
				proto_tree_add_text(parameter_tree, parameter_tvb, offset + 2, 17,
				    "DSP = %s", tvb_bytes_to_str(parameter_tvb, offset + 2, 17));
				proto_tree_add_item(parameter_tree, hf_nsap_ipv4_addr, parameter_tvb, offset + 2, 4, FALSE);
			}
			
			break;
		case 0x45:	/
		case 0xC3:	/
			proto_tree_add_text(parameter_tree, parameter_tvb, offset, 9,
			    "IDP = %s", tvb_bytes_to_str(parameter_tvb, offset, 9));

			proto_tree_add_uint(parameter_tree, hf_afi, parameter_tvb, offset, 1, afi );
	
			proto_tree_add_text(parameter_tree, parameter_tvb, offset + 1, 8,
			    "IDI = %s", tvb_bytes_to_str(parameter_tvb, offset + 1, 8));
			offset = offset +1;
			/
			cc_offset = offset;
			address_digit_pair = tvb_get_guint8(parameter_tvb, cc_offset);
			while ( address_digit_pair == 0 ) {
				cc_offset = cc_offset + 1;
				address_digit_pair = tvb_get_guint8(parameter_tvb, cc_offset);
			}
			cc = tvb_get_ntohs(parameter_tvb, cc_offset);
			if (( address_digit_pair & 0xf0 ) != 0 )
				cc = cc >> 4;

			switch ( cc & 0x0f00 ) {
		
			case 0x0 :
				cc_length = 1;	
				break;

			case 0x0100 : cc_length = 1;	
				break;

			case 0x0200 : 
				switch ( cc & 0x00f0 ) {
					case 0 : 
					case 7 : 
						cc_length = 2;	
						break;
					default : 
						cc_length = 3;
						break;
					}
				break;
				
			case 0x0300 :
				switch ( cc & 0x00f0 ) {
					case 0 : 
					case 0x10 : 
					case 0x20 : 
					case 0x30 : 
					case 0x40 : 
					case 0x60 : 
					case 0x90 : 
						cc_length = 2;
						break;	
					default :
						cc_length = 3;
						break;
				}
				break;
			case 0x0400 : 
				switch ( cc & 0x00f0 ) {
					case 0x20 :
						cc_length = 3;
						break;	
				default :
					cc_length = 2;
					break;
				}
				break;

			case 0x0500 : 
				switch ( cc & 0x00f0 ) {
					case 0 : 
					case 0x90 :
						cc_length = 3;
						break;	
				default :
					cc_length = 2;
					break;
				}
				break;

			case 0x0600 : 
				switch ( cc & 0x00f0 ) {
					case 0x70 : 
					case 0x80 : 
					case 0x90 :
						cc_length = 3; 
						break;	
					default :
						cc_length = 2;
						break;
				}
				break;

			case 0x0700 : cc_length = 1;	
				break;

			case 0x0800 : 
				switch ( cc & 0x00f0 ) {
					case 0x10: 
					case 0x20: 
					case 0x40: 
					case 0x60:
						cc_length = 2; 
						break;	
				default :
					cc_length = 3;
					break;
				}
				break;

			case 0x0900 : 
				switch ( cc & 0x00f0 ) {
					case 0 : 
					case 0x10 : 
					case 0x20 :  
					case 0x30 :  
					case 0x40 :  
					case 0x50 :  
					case 0x80 : 
						cc_length = 2; 
						break;	
					default : 
						cc_length = 3;
						break;
				}
				break;

			default: 
				break;
			}/
			switch ( cc_length ) {
			case 0x1 :	cc = cc >> 8;
				length = 1;
				break;
			case 0x2 :	cc = cc >> 4;
				length = 1;
				break;
			default:
				length = 2;
				break;
			}/
			proto_tree_add_text(parameter_tree,parameter_tvb, cc_offset, length,"Country Code: %x %s length %u",cc,
					val_to_str(cc,E164_country_code_value,"unknown (%x)"),cc_length);
			switch ( cc ) {
			case 0x882 :
					id_code = tvb_get_ntohs(parameter_tvb, cc_offset + 1);
					id_code = (id_code & 0x0fff) >> 4;
					proto_tree_add_text(parameter_tree,parameter_tvb, (cc_offset + 1), 2,"Identification Code: %x %s ",id_code,
						val_to_str(id_code,E164_International_Networks_vals,"unknown (%x)"));
				break;
			default:;
			}
			proto_tree_add_text(parameter_tree,parameter_tvb, cc_offset, length,"DSP length %u(len %u -9 )",(len-9),len );

			proto_tree_add_item(parameter_tree, hf_bicc_nsap_dsp, parameter_tvb, offset + 8, (len - 9),FALSE);
	
			break;
		default:
		 	proto_tree_add_uint(parameter_tree, hf_afi, parameter_tvb, offset, len, afi );
		}/

}
