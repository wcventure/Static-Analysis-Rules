void 
isis_dissect_area_address_clv(tvbuff_t *tvb, proto_tree *tree, int offset,
	int length)
{	
	/
	static char	net[33];
	char		*sbuf = net;
	int		arealen,area_idx;

	while ( length > 0 ) {
		arealen = tvb_get_guint8(tvb, offset);
		length--;
		if (length<=0) {
			isis_dissect_unknown(tvb, tree, offset,
				"short address (no length for payload)");
			return;
		}
		if ( arealen > length) {
			isis_dissect_unknown(tvb, tree, offset,
				"short address, packet say %d, we have %d left",
				arealen, length );
			return;
		}

		/
 
                sbuf = net;
                for (area_idx = 0; area_idx < arealen; area_idx++) {
                    sbuf+=sprintf(sbuf, "%02x", tvb_get_guint8(tvb, offset+area_idx+1));
                    if (((area_idx & 1) == 0) && (area_idx + 1 < arealen)) {
			sbuf+=sprintf(sbuf, ".");
                    }
                }
                *(sbuf) = '\0';

		/
		if ( tree ) {
			proto_tree_add_text ( tree, tvb, offset, arealen + 1,  
				"Area address (%d): %s", arealen, net );
		}
		offset += arealen + 1;
		length -= arealen;	/
	}
}
