static char *
file_rename_error_message(int err)
{
  char *errmsg;
  static char errmsg_errno[1024+1];

  switch (err) {

  case ENOENT:
    errmsg = "The path to the file \"%s\" does not exist.";
    break;

  case EACCES:
    errmsg = "You do not have permission to move the capture file to \"%s\".";
    break;

  default:
    sprintf(errmsg_errno, "The file \"%%s\" could not be moved: %s.",
				wtap_strerror(err));
    errmsg = errmsg_errno;
    break;
  }
  return errmsg;
}
