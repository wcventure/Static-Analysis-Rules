void
rtpproxy_add_parameter(proto_tree *rtpproxy_tree, tvbuff_t *tvb, guint begin, guint realsize)
{
	proto_item *ti;
	proto_tree *another_tree = NULL;
	guint offset = 0;
	guint new_offset = 0;
	gint i = 0;
	guint pt = 0;
	gchar** codecs = NULL;
	guint codec_len;
	guint8* rawstr = NULL;

	/
	/
	rawstr = tvb_get_string(wmem_packet_scope(), tvb, begin, realsize);

	while(offset < realsize){
		ti = proto_tree_add_item(rtpproxy_tree, hf_rtpproxy_command_parameter, tvb, begin + offset, 1, ENC_NA);
		offset++; /
		switch (g_ascii_tolower(tvb_get_guint8(tvb, begin+offset-1)))
		{
			/
			case 'c':
				new_offset = (gint)strspn(rawstr+offset, "0123456789,");
				another_tree = proto_item_add_subtree(ti, ett_rtpproxy_command_parameters_codecs);
				codecs = g_strsplit(tvb_get_string(wmem_packet_scope(), tvb, begin+offset, new_offset), ",", 0);
				while(codecs[i]){
					/
					codec_len = (guint)strlen(codecs[i]);
					proto_tree_add_item(another_tree, hf_rtpproxy_command_parameter_codec, tvb, begin+offset, codec_len, ENC_ASCII | ENC_NA);
					offset += codec_len;
					if(codecs[i+1])
						offset++; /
					i++;
				};
				g_strfreev(codecs);
				break;
			case 'l':
				new_offset = (gint)strspn(rawstr+offset, "0123456789.");
				another_tree = proto_item_add_subtree(ti, ett_rtpproxy_command_parameters_local);
				proto_tree_add_item(another_tree, hf_rtpproxy_command_parameter_local, tvb, begin+offset, new_offset, ENC_ASCII | ENC_NA);
				offset += new_offset;
				break;
			case 'r':
				new_offset = (gint)strspn(rawstr+offset, "0123456789.");
				another_tree = proto_item_add_subtree(ti, ett_rtpproxy_command_parameters_remote);
				proto_tree_add_item(another_tree, hf_rtpproxy_command_parameter_remote, tvb, begin+offset, new_offset, ENC_ASCII | ENC_NA);
				offset += new_offset;
				break;
			case 'z':
				new_offset = (gint)strspn(rawstr+offset, "0123456789");
				another_tree = proto_item_add_subtree(ti, ett_rtpproxy_command_parameters_repacketize);
				proto_tree_add_item(another_tree, hf_rtpproxy_command_parameter_repacketize, tvb, begin+offset, new_offset, ENC_ASCII | ENC_NA);
				offset += new_offset;
				break;
			/
			case 'd':
				new_offset = (gint)strspn(rawstr+offset, "0123456789");
				another_tree = proto_item_add_subtree(ti, ett_rtpproxy_command_parameters_dtmf);
				proto_tree_add_item(another_tree, hf_rtpproxy_command_parameter_dtmf, tvb, begin+offset, new_offset, ENC_ASCII | ENC_NA);
				if(rtpproxy_establish_conversation){
					pt = (guint)strtoul(tvb_format_text(tvb,begin+offset,new_offset),NULL,10);
					dissector_add_uint("rtp.pt", pt, rtp_events_handle);
				}
				offset += new_offset;
				break;
			case 'm':
				new_offset = (gint)strspn(rawstr+offset, "0123456789=,");
				/
				offset += new_offset;
				break;
			case 'p':
				another_tree = proto_item_add_subtree(ti, ett_rtpproxy_command_parameters_proto);
				proto_tree_add_item(another_tree, hf_rtpproxy_command_parameter_proto, tvb, begin+offset, 1, ENC_NA);
				offset++;
				break;
			case 't':
				new_offset = (gint)strspn(rawstr+offset, "0123456789");
				another_tree = proto_item_add_subtree(ti, ett_rtpproxy_command_parameters_transcode);
				proto_tree_add_item(another_tree, hf_rtpproxy_command_parameter_transcode, tvb, begin+offset, new_offset, ENC_ASCII | ENC_NA);
				offset += new_offset;
				break;
			case 'v':
				another_tree = proto_item_add_subtree(ti, ett_rtpproxy_command_parameters_acc);
				proto_tree_add_item(another_tree, hf_rtpproxy_command_parameter_acc, tvb, begin+offset, 1, ENC_NA);
				offset++;
				break;
			default:
				break;
		}
	}
}
