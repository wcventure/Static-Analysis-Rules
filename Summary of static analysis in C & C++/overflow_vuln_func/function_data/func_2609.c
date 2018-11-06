static void add_user_command(char *optarg)
{
    cmdline = g_realloc(cmdline, ++ncmdline * sizeof(char *));
    cmdline[ncmdline-1] = optarg;
}
