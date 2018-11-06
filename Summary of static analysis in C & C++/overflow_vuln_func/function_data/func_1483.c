guint
packet_mpeg_sect_header(tvbuff_t *tvb, guint offset,
			proto_tree *tree, guint *sect_len, gboolean *ssi)
{
	return packet_mpeg_sect_header_extra(tvb, offset, tree, sect_len,
					     NULL, ssi, NULL);
}
