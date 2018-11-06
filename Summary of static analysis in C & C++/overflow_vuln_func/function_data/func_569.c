static int
dissect_corosynec_totemnet_with_decryption(tvbuff_t *tvb,
                                           packet_info *pinfo, proto_tree *parent_tree,
                                           gboolean check_crypt_type,
                                           const gchar* key_for_trial)
{
  unsigned char  keys[48];
  sober128_prng     keygen_prng_state;
  sober128_prng     stream_prng_state;
  unsigned char *hmac_key       = &keys[32];
  unsigned char *cipher_key     = &keys[16];
  unsigned char *initial_vector = &keys[0];
  unsigned char  digest_comparison[HMAC_HASH_SIZE];

  int            io_len;
  guint8        *io_base;

#define PRIVATE_KEY_LEN_MAX 256
  gchar          private_key[PRIVATE_KEY_LEN_MAX];
  gsize          private_key_len;
  unsigned char* hash_digest;
  unsigned char* salt;

  io_len = tvb_reported_length(tvb) - (check_crypt_type? 1: 0);
  if (io_len < HMAC_HASH_SIZE + SALT_SIZE) {
    return 0;
  }

  io_base = (guint8 *)tvb_memdup(pinfo->pool, tvb, 0, io_len + (check_crypt_type? 1: 0));
  if (check_crypt_type &&
      ( io_base[io_len] != TOTEM_CRYPTO_SOBER )) {
    return 0;
  }

  hash_digest = io_base;
  salt        = io_base + HMAC_HASH_SIZE;


  memset(private_key, 0, sizeof(private_key));

  private_key_len = (strlen(key_for_trial)+4) & 0xFC;
  if (private_key_len > PRIVATE_KEY_LEN_MAX)
    private_key_len = PRIVATE_KEY_LEN_MAX;
  g_strlcpy(private_key, key_for_trial, private_key_len);

  /
  memset (keys, 0, sizeof(keys));
  sober128_start (&keygen_prng_state);
  sober128_add_entropy(private_key,
                                  (unsigned long)private_key_len, &keygen_prng_state);
  sober128_add_entropy (salt, SALT_SIZE, &keygen_prng_state);
  sober128_read (keys, sizeof (keys), &keygen_prng_state);

  /
  sober128_start (&stream_prng_state);
  sober128_add_entropy (cipher_key, 16, &stream_prng_state);
  sober128_add_entropy (initial_vector, 16, &stream_prng_state);

  /
  sha1_hmac(hmac_key, 16,
            io_base + HMAC_HASH_SIZE, io_len - HMAC_HASH_SIZE,
            digest_comparison);

  if (memcmp (digest_comparison, hash_digest, HMAC_HASH_SIZE) != 0)
      return 0;

  /

  sober128_read (io_base + HMAC_HASH_SIZE + SALT_SIZE,
                            io_len - (HMAC_HASH_SIZE + SALT_SIZE),
                            &stream_prng_state);


  /
  {
    tvbuff_t *decrypted_tvb;
    tvbuff_t *next_tvb;


    decrypted_tvb = tvb_new_real_data(io_base, io_len, io_len);

    tvb_set_child_real_data_tvbuff(tvb, decrypted_tvb);
    add_new_data_source(pinfo, decrypted_tvb, "Decrypted Data");


    dissect_corosync_totemnet_security_header(decrypted_tvb, pinfo, parent_tree,
                                              check_crypt_type, key_for_trial);

    next_tvb = tvb_new_subset(decrypted_tvb,
                              HMAC_HASH_SIZE + SALT_SIZE,
                              io_len - (HMAC_HASH_SIZE + SALT_SIZE),
                              io_len - (HMAC_HASH_SIZE + SALT_SIZE));

    return call_dissector(corosync_totemsrp_handle, next_tvb, pinfo, parent_tree) + HMAC_HASH_SIZE + SALT_SIZE;
  }
}
