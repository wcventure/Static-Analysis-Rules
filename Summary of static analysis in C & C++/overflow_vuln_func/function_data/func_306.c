static hashmanuf_t *manuf_hash_new_entry(const guint8 *addr, char* name)
{
    int    *manuf_key;
    hashmanuf_t *manuf_value;
    char *endp;

    /
    manuf_key = (int *)g_new(int, 1);
    *manuf_key = (int)((addr[0] << 16) + (addr[1] << 8) + addr[2]);
    manuf_value = g_new(hashmanuf_t, 1);

    memcpy(manuf_value->addr, addr, 3);
    manuf_value->status = (name != NULL) ? HASHETHER_STATUS_RESOLVED_NAME : HASHETHER_STATUS_UNRESOLVED;
    if (name != NULL) {
        g_strlcpy(manuf_value->resolved_name, name, MAXNAMELEN);
        manuf_value->status = HASHETHER_STATUS_RESOLVED_NAME;
    }
    else {
        manuf_value->status = HASHETHER_STATUS_UNRESOLVED;
        manuf_value->resolved_name[0] = '\0';
    }
    /
    endp = bytes_to_hexstr_punct(manuf_value->hexaddr, addr, sizeof(manuf_value->hexaddr), ':');
    *endp = '\0';

    g_hash_table_insert(manuf_hashtable, manuf_key, manuf_value);
    return manuf_value;
}
