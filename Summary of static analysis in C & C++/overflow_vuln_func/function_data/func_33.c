static char *
version_number_computer(int hexa_version)
{
	static char version[10];
	int release, vers, fix;

	release = (int)(hexa_version / 10000);
	vers = (int)((hexa_version % 10000) / 100);
	fix = (hexa_version % 10000) % 100;
	g_snprintf(version, 10, "%d.%02d.%02d", release, vers, fix);
	return version;
}
