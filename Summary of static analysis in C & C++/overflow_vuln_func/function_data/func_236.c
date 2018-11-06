static gboolean
bytes_from_unparsed(fvalue_t *fv, char *s, gboolean allow_partial_value _U_, LogFunc logfunc)
{
	GByteArray	*bytes;
	guint8		val;
	guchar		*p, *q, *punct;
	char		two_digits[3];
	char		one_digit[2];
	gboolean	fail = FALSE;

	bytes = g_byte_array_new();

	p = (guchar *)s;
	while (*p) {
		q = p+1;
		if (*q && isxdigit(*p) && isxdigit(*q)) {
			two_digits[0] = *p;
			two_digits[1] = *q;
			two_digits[2] = '\0';

			/
			val = (guint8) strtoul(two_digits, NULL, 16);
			g_byte_array_append(bytes, &val, 1);
			punct = q + 1;
			if (*punct) {
				/
				if (is_byte_sep(*punct)) {
					p = punct + 1;
					continue;
				}
				else {
					fail = TRUE;
					break;
				}
			}
			else {
				p = punct;
				continue;
			}
		}
		else if (*q && isxdigit(*p) && is_byte_sep(*q)) {
			one_digit[0] = *p;
			one_digit[1] = '\0';

			/
			val = (guint8) strtoul(one_digit, NULL, 16);
			g_byte_array_append(bytes, &val, 1);
			p = q + 1;
			continue;
		}
		else if (!*q && isxdigit(*p)) {
			one_digit[0] = *p;
			one_digit[1] = '\0';

			/
			val = (guint8) strtoul(one_digit, NULL, 16);
			g_byte_array_append(bytes, &val, 1);
			p = q;
			continue;
		}
		else {
			fail = TRUE;
			break;
		}
	}

	if (fail) {
		if (logfunc != NULL)
			logfunc("\"%s\" is not a valid byte string.", s);
		g_byte_array_free(bytes, TRUE);
		return FALSE;
	}

	/
	bytes_fvalue_free(fv);

	fv->value.bytes = bytes;

	return TRUE;
}
