static void
ssl_parse(void)
{
    ep_stack_t tmp_stack;
    SslAssociation *tmp_assoc;
    FILE *ssl_keys_file;
    struct stat statb;
    size_t size;
    gchar *tmp_buf;
    size_t nbytes;
    gboolean read_failed;

    ssl_set_debug(ssl_debug_file_name);

    if (ssl_key_hash)
    {
        g_hash_table_foreach(ssl_key_hash, ssl_private_key_free, NULL);
        g_hash_table_destroy(ssl_key_hash);
    }

    /
    tmp_stack = ep_stack_new();
    g_tree_foreach(ssl_associations, ssl_assoc_from_key_list, tmp_stack);
    while ((tmp_assoc = ep_stack_pop(tmp_stack)) != NULL) {
        ssl_association_remove(ssl_associations, tmp_assoc);
    }

    /
    ssl_key_hash = g_hash_table_new(ssl_private_key_hash,ssl_private_key_equal);

    if (ssl_keys_list && (ssl_keys_list[0] != 0))
    {
        if (file_exists(ssl_keys_list)) {
            if ((ssl_keys_file = ws_fopen(ssl_keys_list, "r"))) {
                read_failed = FALSE;
                fstat(fileno(ssl_keys_file), &statb);
                size = (size_t)statb.st_size;
                tmp_buf = ep_alloc0(size + 1);
                nbytes = fread(tmp_buf, 1, size, ssl_keys_file);
                if (ferror(ssl_keys_file)) {
                    report_read_failure(ssl_keys_list, errno);
                    read_failed = TRUE;
                }
                fclose(ssl_keys_file);
                tmp_buf[nbytes] = '\0';
                if (!read_failed)
                    ssl_parse_key_list(tmp_buf,ssl_key_hash,ssl_associations,ssl_handle,TRUE);
            } else {
                report_open_failure(ssl_keys_list, errno, FALSE);
            }
        } else {
            ssl_parse_key_list(ssl_keys_list,ssl_key_hash,ssl_associations,ssl_handle,TRUE);
        }
    }
    ssl_debug_flush();
}
