static void
snmp_usm_password_to_key_sha1(const guint8 *password, guint passwordlen,
			      const guint8 *engineID, guint engineLength,
			      guint8 *key)
{
	sha1_context	SH;
	guint8		*cp, password_buf[64];
	guint32		password_index = 0;
	guint32		count = 0, i;

	sha1_starts(&SH); /

	/
	/
	/
	while (count < 1048576) {
		cp = password_buf;
		if (passwordlen != 0) {
			for (i = 0; i < 64; i++) {
				/
				/
				/
				/
				*cp++ = password[password_index++ % passwordlen];
			}
		} else {
			*cp = 0;
		}
		sha1_update (&SH, password_buf, 64);
		count += 64;
	}
	sha1_finish(&SH, key);

	/
	/
	/
	/
	/
	/

	sha1_starts(&SH);
	sha1_update(&SH, key, 20);
	sha1_update(&SH, engineID, engineLength);
	sha1_update(&SH, key, 20);
	sha1_finish(&SH, key);
	return;
 }
