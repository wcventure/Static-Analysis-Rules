void
isis_dissect_nlpid_clv(tvbuff_t *tvb, proto_tree *tree, int offset, int length)
{
	char sbuf[256*6];
	char *s = sbuf;
	int hlen = length;
	int old_offset = offset;

	if ( !tree ) return;		/

	while ( length-- > 0 ) {
		if (s != sbuf ) {
			s += sprintf ( s, ", " );
		}
		s += sprintf ( s, "%s (0x%02x)",
		    val_to_str(tvb_get_guint8(tvb, offset), nlpid_vals,
			"Unknown"), tvb_get_guint8(tvb, offset));
		offset++;
	}

	if ( hlen == 0 ) {
		sprintf ( sbuf, "--none--" );
	}

	proto_tree_add_text ( tree, tvb, old_offset, hlen,
			"NLPID(s): %s", sbuf );
}
