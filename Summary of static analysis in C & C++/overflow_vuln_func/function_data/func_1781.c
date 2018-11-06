#ifdef HAVE_SPRINT_VALUE
static gchar *
check_var_length(guint vb_length, guint required_length)
{
	gchar *buf;
	static const char badlen_fmt[] = "Length is %u, should be %u";

	if (vb_length != required_length) {
		/
		buf = g_malloc(sizeof badlen_fmt + 10 + 10);
		sprintf(buf, badlen_fmt, vb_length, required_length);
		return buf;
	}
	return NULL;	/
}
