static void
dissect_manolito(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	struct MANOLITO_HEADER header;
	unsigned int offset;

	/
	proto_item *ti;
	proto_tree *manolito_tree;
	char* packet_type = 0;

	/
	if (check_col(pinfo->cinfo, COL_PROTOCOL)) 
		col_set_str(pinfo->cinfo, COL_PROTOCOL, "MANOLITO");
    
	ti = proto_tree_add_item(tree, proto_manolito, tvb, 0, -1, FALSE);

	manolito_tree = proto_item_add_subtree(ti, ett_manolito);

	/
	header.checksum = tvb_get_ntohl(tvb, 0);
	header.seqno = tvb_get_ntohl(tvb, 4);
	tvb_memcpy(tvb, (guint8 *)&header.src, 8, 4);
	tvb_memcpy(tvb, (guint8 *)&header.dest, 12, 4);
	if (tvb_reported_length(tvb) == 19)
	{
		header.options = (tvb_get_ntohs(tvb, 16) << 8) +
				tvb_get_guint8(tvb, 18);
		packet_type = "Ping (truncated)";
	} else {
		header.options = tvb_get_ntohl(tvb, 16);
	}

	proto_tree_add_item(manolito_tree,
	    hf_manolito_checksum, tvb, 0, 4, header.checksum);
	proto_tree_add_item(manolito_tree,
	    hf_manolito_seqno, tvb, 4, 4, header.seqno);

	proto_tree_add_ipv4(manolito_tree,
	    hf_manolito_src, tvb, 8, 4, header.src);

	proto_tree_add_ipv4(manolito_tree,
	    hf_manolito_dest, tvb, 12, 4, header.dest);

	proto_tree_add_item(manolito_tree,
	    hf_manolito_options, tvb, 16, 4, header.options);


	if (tvb_reported_length(tvb) <= 20)      /
	{
		if (check_col(pinfo->cinfo, COL_INFO)) 
		{
			col_set_str(pinfo->cinfo, COL_INFO, "Ping");
		}
	} else {
		offset = 20;            /

	 	/
		/
		/
		do 
		{
			guint16 field_name;      /
			guint8 dtype;            /
			guint8 length;           /
			guint8* data;            /
			int start;               /
			char field_name_str[3];  /
			char* longname;          /

			start = offset;

			/
			field_name = tvb_get_ntohs(tvb, offset);
			offset += 2;

			/
			/
			if (field_name == 0x434b)    /
				packet_type = "Search Hit";
			if (field_name == 0x4e43)    /
				packet_type = "User Information";
			if (field_name == 0x464e)    /
				packet_type = "Search Query";
			if (field_name == 0x4944)    /
				packet_type = "Search Query (by hash)";
			if (field_name == 0x5054)    /
				packet_type = "Download Request";
			if (field_name == 0x4d45)    /
				packet_type = "Chat";

			if (tvb_reported_length(tvb) == 20)   /
				packet_type = "Ping";

			/
			switch(field_name)
			{
			case 0x5346: longname = "Shared Files"; break;    /
			case 0x534b: longname = "Shared Kilobytes";break; /
			case 0x4e49: longname = "Network ID"; break;      /
			case 0x4e43: longname = "Num. Connections";break; /
			case 0x4356: longname = "Client Version"; break;  /
			case 0x564c: longname = "Velocity"; break;        /
			case 0x464e: longname = "Filename"; break;        /
			case 0x464c: longname = "File Length"; break;     /
			case 0x4252: longname = "Bit Rate"; break;        /
			case 0x4643: longname = "Frequency"; break;       /
			case 0x5354: longname = "???"; break;             /
			case 0x534c: longname = "Song Length (s)"; break; /
			case 0x434b: longname = "Checksum"; break;    /
			case 0x4e4e: longname = "Nickname"; break;        /
			case 0x434e: longname = "Client Name"; break;     /
			case 0x5054: longname = "Port"; break;            /
			case 0x484e: longname = "???"; break;             /
			case 0x4d45: longname = "Message"; break;         /
			case 0x4944: longname = "Identification"; break;  /
			case 0x4144: longname = "???"; break;             /
			default: longname = "unknown"; break;
			}

			/
#define MANOLITO_STRING		1
#define MANOLITO_INTEGER	0
			dtype = tvb_get_guint8(tvb, offset);
			length = tvb_get_guint8(tvb, ++offset);

			/
			g_assert(length<0xff); /
			data = malloc(length + 1);
			tvb_memcpy(tvb, (guint8*)data, ++offset, length);
			offset += length; 

			/
                        /
			field_name_str[0] = g_htons(field_name) & 0x00ff;
			field_name_str[1] = (g_htons(field_name) & 0xff00) >> 8;
			field_name_str[2] = 0;

			if (dtype == MANOLITO_STRING) 
			{
				data[length] = 0;
				proto_tree_add_text(manolito_tree, tvb, start,
					offset - start, "%s (%s): %s",
					(char*)field_name_str, longname, data);
			} else if (dtype == MANOLITO_INTEGER) {
			 	int n = 0;

				/
				switch(length)
				{
				case 5: n += data[4] << ((length - 5) * 8);
				case 4: n += data[3] << ((length - 4) * 8);
				case 3: n += data[2] << ((length - 3) * 8);
				case 2: n += data[1] << ((length - 2) * 8);
				case 1: n += data[0] << ((length - 1) * 8);
				}
				proto_tree_add_text(manolito_tree, tvb, start,
					offset - start, "%s (%s): %d",
					(char*)field_name_str, longname, n);
			} else {
				proto_tree_add_text(manolito_tree, tvb, start,
					offset - start, "unknown type %d", dtype);
			}
			free(data);

		} while(offset < tvb_reported_length(tvb));

	}

	if (packet_type && check_col(pinfo->cinfo, COL_INFO))
	{
		col_set_str(pinfo->cinfo, COL_INFO, packet_type);
	}
}
