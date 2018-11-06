static int
dissect_corosync_totemnet_security_header(tvbuff_t *tvb,
                                          packet_info *pinfo, proto_tree *parent_tree,
                                          gboolean check_crypt_type,
                                          const gchar* key)
{
  proto_item *item;
  proto_tree *tree;

  col_set_str(pinfo->cinfo, COL_PROTOCOL, "COROSYNC/TOTEMNET");
  col_clear(pinfo->cinfo, COL_INFO);

  if (parent_tree)
    {
      item = proto_tree_add_item(parent_tree, proto_corosync_totemnet, tvb, 0,
                                 -1, ENC_NA);
      tree = proto_item_add_subtree(item, ett_corosync_totemnet_security_header);

      proto_tree_add_item(tree,
                          hf_corosync_totemnet_security_header_hash_digest,
                          tvb, 0, HMAC_HASH_SIZE, ENC_NA);
      proto_tree_add_item(tree,
                          hf_corosync_totemnet_security_header_salt,
                          tvb, HMAC_HASH_SIZE, SALT_SIZE, ENC_NA);

      if (check_crypt_type)
        {
          int io_len = tvb_reported_length(tvb);
          proto_item * key_item;

          proto_tree_add_item(tree,
                              hf_corosync_totemnet_security_crypto_type,
                              tvb, io_len - 1, 1, ENC_BIG_ENDIAN);
          key_item = proto_tree_add_string(tree,
                                           hf_corosync_totemnet_security_crypto_key,
                                           tvb, 0, 0, key);
          PROTO_ITEM_SET_GENERATED(key_item);
        }
    }
  return HMAC_HASH_SIZE + SALT_SIZE;
}
