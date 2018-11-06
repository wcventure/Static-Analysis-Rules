static int
rfc3986_parse_port(URI *uri, const char **str)
{
    const char *cur = *str;

    if (ISA_DIGIT(cur)) {
	if (uri != NULL)
	    uri->port = 0;
	while (ISA_DIGIT(cur)) {
	    if (uri != NULL)
		uri->port = uri->port * 10 + (*cur - '0');
	    cur++;
	}
	*str = cur;
	return(0);
    }
    return(1);
}
