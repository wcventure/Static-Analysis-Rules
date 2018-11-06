static void
snmp_usm_password_to_key_md5(const guint8 *password, guint passwordlen,
			     const guint8 *engineID, guint   engineLength,
			     guint8 *key)
{
	md5_state_t     MD;
	guint8     *cp, password_buf[64];
	guint32      password_index = 0;
	guint32      count = 0, i;
	guint8		key1[16];
	md5_init(&MD);   /

	/
	/
	/
	while (count < 1048576) {
		cp = password_buf;
		for (i = 0; i < 64; i++) {
			/
			/
			/
			/
			*cp++ = password[password_index++ % passwordlen];
		}
		md5_append(&MD, password_buf, 64);
		count += 64;
	}
	md5_finish(&MD, key1);          /

	/
	/
	/
	/
	/
	/

	md5_init(&MD);
	md5_append(&MD, key1, 16);
	md5_append(&MD, engineID, engineLength);
	md5_append(&MD, key1, 16);
	md5_finish(&MD, key);

	return;
}
