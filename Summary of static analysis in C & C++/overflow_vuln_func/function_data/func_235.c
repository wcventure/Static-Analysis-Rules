static gboolean
is_byte_sep(guint8 c)
{
	return (c == '-' || c == ':' || c == '.');
}
