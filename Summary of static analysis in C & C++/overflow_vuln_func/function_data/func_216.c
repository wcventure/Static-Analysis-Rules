void
ssl_parse_key_list(const gchar * keys_list, GHashTable *key_hash, GTree* associations, dissector_handle_t handle, gboolean tcp)
{
    gchar* end;
    gchar* start;
    gchar* tmp;
    guchar* ip;
    SslService* service;
    Ssl_private_key_t * private_key, *tmp_private_key;
    FILE* fp;

    start = g_strdup(keys_list);
    tmp = start;
    ssl_debug_printf("ssl_init keys string:\n%s\n", start);
    do {
        int read_index, write_index;
        gchar* addr, *port, *protocol, *filename, *cert_passwd;

        addr = start;
        /
        end = strpbrk(start, ";\n\r");
        if (end) {
            *end = 0;
            start = end+1;
        }

        /
        if (addr[0] == '#') continue;

        /
        ssl_debug_printf("ssl_init found host entry %s\n", addr);
        port = strchr(addr, ',');
        if (!port)
        {
            ssl_debug_printf("ssl_init entry malformed can't find port in '%s'\n", addr);
            continue;
        }
        *port = 0;
        port++;

        protocol = strchr(port,',');
        if (!protocol)
        {
            ssl_debug_printf("ssl_init entry malformed can't find protocol in %s\n", port);
            continue;
        }
        *protocol=0;
        protocol++;

        filename = strchr(protocol,',');
        if (!filename)
        {
            ssl_debug_printf("ssl_init entry malformed can't find filename in %s\n", protocol);
            continue;
        }
        *filename=0;
        filename++;

        cert_passwd = strchr(filename,',');
        if (cert_passwd)
        {
            *cert_passwd=0;
            cert_passwd++;
        }

        /
        service = g_malloc(sizeof(SslService) + 4);
        service->addr.type = AT_IPv4;
        service->addr.len = 4;
        service->addr.data = ip = ((guchar*)service) + sizeof(SslService);

        /
        read_index = 0;
        write_index = 0;

        while(addr[read_index]) {
            if (addr[read_index] != ' ') {
                addr[write_index] = addr[read_index];
                write_index++;
            }
            read_index++;
        }
        addr[write_index] = 0;

        if ( !strcmp("any", addr) || !strcmp("ANY", addr) ) {
            ip[0] = 0;
            ip[1] = 0;
            ip[2] = 0;
            ip[3] = 0;
        } else {
	    guint tmp0, tmp1, tmp2, tmp3;

            sscanf(addr, "%u.%u.%u.%u", &tmp0, &tmp1, &tmp2, &tmp3);
	    ip[0] = (guchar)tmp0;
	    ip[1] = (guchar)tmp1;
	    ip[2] = (guchar)tmp2;
	    ip[3] = (guchar)tmp3;
        }

        if(!strcmp("start_tls", port)) {
            service->port = 0;
        } else {
            service->port = atoi(port);
        }
        ssl_debug_printf("ssl_init addr '%u.%u.%u.%u' port '%d' filename '%s' password(only for p12 file) '%s'\n",
                         ip[0], ip[1], ip[2], ip[3], service->port, filename, cert_passwd ? cert_passwd : "(null)");

        /
        fp = ws_fopen(filename, "rb");
        if (!fp) {
            fprintf(stderr, "can't open file %s \n",filename);
            continue;
        }

        if (!cert_passwd) {
            private_key = ssl_load_key(fp);
        }
        else
        {
            private_key = ssl_load_pkcs12(fp,cert_passwd);
        }
        /
        if (!private_key) {
            fprintf(stderr,"can't load private key from %s\n",
                    filename);
            fclose(fp);
            continue;
        }

        fclose(fp);

        ssl_debug_printf("ssl_init private key file %s successfully loaded\n",filename);

        /
        tmp_private_key = g_hash_table_lookup(key_hash, service);
        if (tmp_private_key) {
            g_hash_table_remove(key_hash, service);
            ssl_free_key(tmp_private_key);
        }
        g_hash_table_insert(key_hash, service, private_key);

        ssl_association_add(associations, handle, service->port, protocol, tcp, TRUE);

    } while (end != NULL);
    g_free(tmp);
}
