    GetTempPath(MAX_PATH, temp_dir);
    GetTempFileName(temp_dir, "qem", 0, filename);
}
#else
void get_tmp_filename(char *filename, int size)
{
    int fd;
    const char *tmpdir;
    /
    tmpdir = getenv("TMPDIR");
    if (!tmpdir)
        tmpdir = "/tmp";
    snprintf(filename, size, "%s/vl.XXXXXX", tmpdir);
    fd = mkstemp(filename);
    close(fd);
}
