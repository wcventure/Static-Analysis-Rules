static snmp_ue_assoc_t*
get_user_assoc(tvbuff_t* engine_tvb, tvbuff_t* user_tvb)
{
	static snmp_ue_assoc_t* a;
	guint given_username_len;
	guint8* given_username;
	guint given_engine_len;
	guint8* given_engine;

	if ( ! (localized_ues || unlocalized_ues ) ) return NULL;

	if (! ( user_tvb && engine_tvb ) ) return NULL;

	given_username_len = tvb_length_remaining(user_tvb,0);
	given_username = ep_tvb_memdup(user_tvb,0,-1);
	given_engine_len = tvb_length_remaining(engine_tvb,0);
	given_engine = ep_tvb_memdup(engine_tvb,0,-1);

	for (a = localized_ues; a; a = a->next) {
		if ( localized_match(a, given_username, given_username_len, given_engine, given_engine_len) ) {
			return a;
		}
	}

	for (a = unlocalized_ues; a; a = a->next) {
		if ( unlocalized_match(a, given_username, given_username_len) ) {
			snmp_ue_assoc_t* n = localize_ue( a, given_engine, given_engine_len );
			CACHE_INSERT(localized_ues,n);
			return n;
		}
	}

	return NULL;
}
