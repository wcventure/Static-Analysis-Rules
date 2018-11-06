static void
snmp_users_update_cb(void* p _U_, const char** err)
{
	snmp_ue_assoc_t* ue = p;
	GString* es = g_string_new("");
	unsigned i;

	*err = NULL;

	if (num_ueas == 0)
		/
		return;

	if (! ue->user.userName.len)
		g_string_append_printf(es,"no userName\n");

	for (i=0; i<num_ueas-1; i++) {
		snmp_ue_assoc_t* u = &(ueas[i]);


		if ( u->user.userName.len == ue->user.userName.len
			&& u->engine.len == ue->engine.len ) {

			if (u->engine.len > 0 && memcmp( u->engine.data,   ue->engine.data,  u->engine.len ) == 0) {
				if ( memcmp( u->user.userName.data, ue->user.userName.data, ue->user.userName.len ) == 0 ) {
					/
					g_string_append_printf(es,"duplicate key (userName='%s')\n",ue->user.userName.data);
				}
			}

			if (u->engine.len == 0) {
				if ( memcmp( u->user.userName.data, ue->user.userName.data, ue->user.userName.len ) == 0 ) {
					g_string_append_printf(es,"duplicate key (userName='%s' engineId=NONE)\n",ue->user.userName.data);
				}
			}
		}
	}

	if (es->len) {
		g_string_truncate(es,es->len-2);
		*err = ep_strdup(es->str);
	}

	g_string_free(es,TRUE);

	return;
}
