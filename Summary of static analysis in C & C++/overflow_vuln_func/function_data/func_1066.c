static char *
file_close_error_message(int err)
{
  char *errmsg;
  static char errmsg_errno[1024+1];

  switch (err) {

  case WTAP_ERR_CANT_CLOSE:
    errmsg = "The file \"%s\" couldn't be closed for some unknown reason.";
    break;

  case WTAP_ERR_SHORT_WRITE:
    errmsg = "Not all the data could be written to the file \"%s\".";
    break;

  case ENOSPC:
    errmsg = "The file \"%s\" could not be saved because there is no space left on the file system.";
    break;

#ifdef EDQUOT
  case EDQUOT:
    errmsg = "The file \"%s\" could not be saved because you are too close to, or over, your disk quota.";
    break;
#endif

  default:
    sprintf(errmsg_errno, "An error occurred while closing the file \"%%s\": %s.",
				wtap_strerror(err));
    errmsg = errmsg_errno;
    break;
  }
  return errmsg;
}
