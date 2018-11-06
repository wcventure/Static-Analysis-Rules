#ifdef _WIN32
void get_tmp_filename(char *filename, int size)
{
    char temp_dir[MAX_PATH];

    GetTempPath(MAX_PATH, temp_dir);
    GetTempFileName(temp_dir, "qem", 0, filename);
}
