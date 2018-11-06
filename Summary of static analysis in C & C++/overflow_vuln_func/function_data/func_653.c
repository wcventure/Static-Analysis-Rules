gboolean
merge_open_in_files(int in_file_count, char *const *in_file_names,
                    merge_in_file_t **in_files, int *err, gchar **err_info,
                    int *err_fileno)
{
  int i, j;
  size_t files_size = in_file_count * sizeof(merge_in_file_t);
  merge_in_file_t *files;
  gint64 size;

  files = (merge_in_file_t *)g_malloc(files_size);
  *in_files = files;

  for (i = 0; i < in_file_count; i++) {
    files[i].filename    = in_file_names[i];
    files[i].wth         = wtap_open_offline(in_file_names[i], WTAP_TYPE_AUTO, err, err_info, FALSE);
    files[i].data_offset = 0;
    files[i].state       = PACKET_NOT_PRESENT;
    files[i].packet_num  = 0;
    if (!files[i].wth) {
      /
      for (j = 0; j < i; j++)
        wtap_close(files[j].wth);
      *err_fileno = i;
      return FALSE;
    }
    size = wtap_file_size(files[i].wth, err);
    if (size == -1) {
      for (j = 0; j <= i; j++)
        wtap_close(files[j].wth);
      *err_fileno = i;
      return FALSE;
    }
    files[i].size = size;
  }
  return TRUE;
}
