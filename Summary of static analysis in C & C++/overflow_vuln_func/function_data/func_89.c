guint find_terminal_port(guint port)
{
	int i;
	for (i=0; i<MAX_TERMINAL_PORTS; i++)
		if (ports[i].port == port)
			return 1;
	return 0;
}
