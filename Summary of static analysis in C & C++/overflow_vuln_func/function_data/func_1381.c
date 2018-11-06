void zbee_sec_add_key_to_keyring(packet_info *pinfo, const guint8 *key)
{
    GSList            **nwk_keyring;
    key_record_t        key_record;
    zbee_nwk_hints_t   *nwk_hints;

    /
    if ( !pinfo->fd->flags.visited && (nwk_hints = (zbee_nwk_hints_t *)p_get_proto_data(wmem_file_scope(), pinfo,
                    proto_get_id_by_filter_name(ZBEE_PROTOABBREV_NWK), 0))) {
        nwk_keyring = (GSList **)g_hash_table_lookup(zbee_table_nwk_keyring, &nwk_hints->src_pan);
        if ( !nwk_keyring ) {
            nwk_keyring = (GSList **)g_malloc0(sizeof(GSList*));
            g_hash_table_insert(zbee_table_nwk_keyring,
                    g_memdup(&nwk_hints->src_pan, sizeof(nwk_hints->src_pan)), nwk_keyring);
        }

        if ( nwk_keyring ) {
            if ( !*nwk_keyring ||
                    memcmp( ((key_record_t *)((GSList *)(*nwk_keyring))->data)->key, &key,
                        ZBEE_APS_CMD_KEY_LENGTH) ) {
                /
                key_record.frame_num = pinfo->num;
                key_record.label = NULL;
                memcpy(&key_record.key, key, ZBEE_APS_CMD_KEY_LENGTH);
                *nwk_keyring = g_slist_prepend(*nwk_keyring, g_memdup(&key_record, sizeof(key_record_t)));
            }
        }
    }
} /
