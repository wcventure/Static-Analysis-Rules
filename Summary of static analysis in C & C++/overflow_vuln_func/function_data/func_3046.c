static void start_children(FFServerStream *feed)
{
    char *pathname;
    char *slash;
    int i;
    size_t cmd_length;

    if (no_launch)
        return;

    cmd_length = strlen(my_program_name);

   /
    if (cmd_length > PATH_LENGTH - 1) {
        http_log("Could not start children. Command line: '%s' exceeds "
                    "path length limit (%d)\n", my_program_name, PATH_LENGTH);
        return;
    }

    pathname = av_strdup (my_program_name);
    if (!pathname) {
        http_log("Could not allocate memory for children cmd line\n");
        return;
    }
   /

    slash = strrchr(pathname, '/');
    if (!slash)
        slash = pathname;
    else
        slash++;
    strcpy(slash, "ffmpeg");

    for (; feed; feed = feed->next) {

        if (!feed->child_argv || feed->pid)
            continue;

        feed->pid_start = time(0);

        feed->pid = fork();
        if (feed->pid < 0) {
            http_log("Unable to create children: %s\n", strerror(errno));
            av_free (pathname);
            exit(EXIT_FAILURE);
        }

        if (feed->pid)
            continue;

        /

        http_log("Launch command line: ");
        http_log("%s ", pathname);

        for (i = 1; feed->child_argv[i] && feed->child_argv[i][0]; i++)
            http_log("%s ", feed->child_argv[i]);
        http_log("\n");

        for (i = 3; i < 256; i++)
            close(i);

        if (!config.debug) {
            if (!freopen("/dev/null", "r", stdin))
                http_log("failed to redirect STDIN to /dev/null\n;");
            if (!freopen("/dev/null", "w", stdout))
                http_log("failed to redirect STDOUT to /dev/null\n;");
            if (!freopen("/dev/null", "w", stderr))
                http_log("failed to redirect STDERR to /dev/null\n;");
        }

        signal(SIGPIPE, SIG_DFL);
        execvp(pathname, feed->child_argv);
        av_free (pathname);
        _exit(1);
    }
    av_free (pathname);
}
