guint oid_encoded2subid(const guint8 *oid_bytes, gint oid_len, guint32** subids_p) {
	gint i;
	guint n = 1;
	guint32 subid = 0;
	gboolean is_first = TRUE;
	guint32* subids;
	
	for (i=0; i<oid_len; i++) { if (! (oid_bytes[i] & 0x80 )) n++; }
	
	*subids_p = subids = ep_alloc(sizeof(guint32)*n);
	
	for (i=0; i<oid_len; i++){
		guint8 byte = oid_bytes[i];
		
		subid <<= 7;
		subid |= byte & 0x7F;
		
		if (byte & 0x80) {
			continue;
		}
		
		if (is_first) {
			guint32 subid0 = 0;
			
			if (subid >= 40) { subid0++; subid-=40; }
			if (subid >= 40) { subid0++; subid-=40; }
			
			*subids++ = subid0;
			
			is_first = FALSE;
		}
		
		*subids++ = subid;
		subid = 0;
	}
	
	return n;
}
