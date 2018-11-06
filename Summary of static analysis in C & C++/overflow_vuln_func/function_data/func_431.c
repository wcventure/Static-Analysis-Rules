const gchar *oid_resolved(guint32 num_subids, guint32* subids) {
	guint matched;
	guint left;
	oid_info_t* oid = oid_get(num_subids, subids, &matched, &left);
	
	while (! oid->name ) {
		if (!(oid = oid->parent)) {
			return oid_subid2string(subids,num_subids);
		}
		left++;
		matched--;
	}
	
	if (left) {
		return ep_strdup_printf("%s.%s",
								oid->name ? oid->name : oid_subid2string(subids,matched),
								oid_subid2string(&(subids[matched]),left));
	} else {
		return oid->name ? oid->name : oid_subid2string(subids,matched);
	}
}
