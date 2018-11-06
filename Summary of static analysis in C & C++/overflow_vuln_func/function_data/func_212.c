static void
dtls_parse(void)
{
  ep_stack_t tmp_stack;
  SslAssociation *tmp_assoc;

  if (dtls_key_hash)
    {
      g_hash_table_foreach(dtls_key_hash, ssl_private_key_free, NULL);
      g_hash_table_destroy(dtls_key_hash);
    }

  /
  tmp_stack = ep_stack_new();
  g_tree_foreach(dtls_associations, ssl_assoc_from_key_list, tmp_stack);
  while ((tmp_assoc = ep_stack_pop(tmp_stack)) != NULL) {
    ssl_association_remove(dtls_associations, tmp_assoc);
  }

  /
  dtls_key_hash = g_hash_table_new(ssl_private_key_hash, ssl_private_key_equal);

  if (dtls_keys_list && (dtls_keys_list[0] != 0))
    {
      ssl_parse_key_list(dtls_keys_list,dtls_key_hash,dtls_associations,dtls_handle,FALSE);
    }

  ssl_set_debug(dtls_debug_file_name);

  dissector_add_handle("sctp.port", dtls_handle);
  dissector_add_handle("udp.port", dtls_handle);
}
