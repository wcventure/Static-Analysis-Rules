static void _dissect_ua_msg(tvbuff_t *tvb,
						packet_info *pinfo,
						proto_tree *tree,
						e_ua_direction direction)
{
	proto_item *ua_msg_item = NULL;
	proto_tree *ua_msg_tree = NULL;
	gint offset = 0;
	gint length = 0;
	gint opcode = 0;
	gint suboffset  = 0;
	address remote_rtp_addr;
	guint32 remote_rtp_port = 0;

	if(tree)
	{
		ua_msg_item = proto_tree_add_protocol_format(tree, proto_ua_msg, tvb, 0, -1,
			"Universal Alcatel Protocol, %s",
			((direction == SYS_TO_TERM) ?
			"System -> Terminal" : "Terminal -> System"));

		ua_msg_tree = proto_item_add_subtree(ua_msg_item, ett_ua_msg);

		while (tvb_offset_exists(tvb, offset))
		{
			length = (tvb_get_guint8(tvb, offset+0) +
				tvb_get_guint8(tvb, offset+1) * 256) + 2;

			opcode = tvb_get_guint8(tvb, offset+2);

			/
			if(setup_conversations_enabled && opcode==0x13 && tvb_get_guint8(tvb, offset+3)==0x01)
			{
				/
				suboffset = offset+5;
				remote_rtp_addr.data = NULL;

				while(suboffset < offset+length)
				{
					switch(tvb_get_guint8(tvb, suboffset))
					{
					case 0x00: /
						{
						/
						break;
						}
					case 0x01: /
						{
						remote_rtp_addr.type=AT_IPv4;
						remote_rtp_addr.len=4;
						remote_rtp_addr.data=tvb_get_ptr(tvb, suboffset+2, 4);
						break;
						}
					case 0x02: /
						{
						remote_rtp_port = tvb_get_ntohs(tvb, suboffset+2);
						break;
						}
					}

				suboffset += tvb_get_guint8(tvb, suboffset+1) + 2;
				}

				if(remote_rtp_addr.data!=NULL && remote_rtp_port!=0)
				{
					if(rtp_handle)
					{
						rtp_add_address(pinfo, &remote_rtp_addr, remote_rtp_port, 0,
						"UA", pinfo->fd->num, 0, NULL);
					}
					if(rtcp_handle)
					{
						rtcp_add_address(pinfo, &remote_rtp_addr, remote_rtp_port+1, 0,
						"UA", pinfo->fd->num);
					}
				}
			}

			uadecode(direction, ua_msg_tree, pinfo, tvb, offset, opcode, length);

			offset += length;
		}
	}
}
