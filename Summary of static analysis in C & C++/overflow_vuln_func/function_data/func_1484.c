guint
packet_mpeg_sect_header_extra(tvbuff_t *tvb, guint offset, proto_tree *tree,
				guint *sect_len, guint *reserved, gboolean *ssi,
				proto_item **items)
{
	guint       tmp;
	guint       len = 0;
	proto_item *pi[PACKET_MPEG_SECT_PI__SIZE];
	gint        i;

	for (i = 0; i < PACKET_MPEG_SECT_PI__SIZE; i++) {
		pi[i] = NULL;
	}

	if (tree) {
		pi[PACKET_MPEG_SECT_PI__TABLE_ID] =
			proto_tree_add_item(tree, hf_mpeg_sect_table_id,
					tvb, offset + len, 1, ENC_BIG_ENDIAN);
	}

	len++;

	if (tree) {
		pi[PACKET_MPEG_SECT_PI__SSI] =
			proto_tree_add_item(tree, hf_mpeg_sect_syntax_indicator,
					tvb, offset + len, 2, ENC_BIG_ENDIAN);

		pi[PACKET_MPEG_SECT_PI__RESERVED] =
			proto_tree_add_item(tree, hf_mpeg_sect_reserved, tvb,
					offset + len, 2, ENC_BIG_ENDIAN);

		pi[PACKET_MPEG_SECT_PI__LENGTH] =
			proto_tree_add_item(tree, hf_mpeg_sect_length, tvb,
					offset + len, 2, ENC_BIG_ENDIAN);
	}

	tmp = tvb_get_ntohs(tvb, offset + len);

	if (sect_len)
		*sect_len = MPEG_SECT_LENGTH_MASK & tmp;

	if (reserved)
		*reserved = (MPEG_SECT_RESERVED_MASK & tmp) >> 12;

	if (ssi)
		*ssi = (MPEG_SECT_SYNTAX_INDICATOR_MASK & tmp);

	if (items) {
		for (i = 0; i < PACKET_MPEG_SECT_PI__SIZE; i++) {
			items[i] = pi[i];
		}
	}

	len += 2;

	return len;
}
