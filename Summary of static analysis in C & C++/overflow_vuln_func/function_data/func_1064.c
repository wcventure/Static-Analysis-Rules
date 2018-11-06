char *
file_read_error_message(int err)
{
  static char errmsg_errno[1024+1];

  sprintf(errmsg_errno, "An error occurred while reading from the file \"%%s\": %s.",
				wtap_strerror(err));
  return errmsg_errno;
}
