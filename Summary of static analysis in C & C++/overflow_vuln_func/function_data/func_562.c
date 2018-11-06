static gboolean
snmp_usm_auth_sha1(snmp_usm_params_t* p _U_, guint8** calc_auth_p, guint* calc_auth_len_p, gchar const** error _U_)
{
	gint msg_len;
	guint8* msg;
	guint auth_len;
	guint8* auth;
	guint8* key;
	guint key_len;
	guint8 *calc_auth;
	guint start;
	guint end;
	guint i;

	if (!p->auth_tvb) {
		*error = "No Authenticator";
		return FALSE;
	}

	key = p->user_assoc->user.authKey.data;
	key_len = p->user_assoc->user.authKey.len;

	if (! key ) {
		*error = "User has no authKey";
		return FALSE;
	}


	auth_len = tvb_captured_length(p->auth_tvb);


	if (auth_len != 12) {
		*error = "Authenticator length wrong";
		return FALSE;
	}

	msg_len = tvb_captured_length(p->msg_tvb);
	if (msg_len <= 0) {
		*error = "Not enough data remaining";
		return FALSE;
	}
	msg = (guint8*)tvb_memdup(wmem_packet_scope(),p->msg_tvb,0,msg_len);

	auth = (guint8*)tvb_memdup(wmem_packet_scope(),p->auth_tvb,0,auth_len);

	start = p->auth_offset - p->start_offset;
	end = 	start + auth_len;

	/
	for ( i = start ; i < end ; i++ ) {
		msg[i] = '\0';
	}

	calc_auth = (guint8*)wmem_alloc(wmem_packet_scope(), 20);

	sha1_hmac(key, key_len, msg, msg_len, calc_auth);

	if (calc_auth_p) *calc_auth_p = calc_auth;
	if (calc_auth_len_p) *calc_auth_len_p = 12;

	return ( memcmp(auth,calc_auth,12) != 0 ) ? FALSE : TRUE;
}
