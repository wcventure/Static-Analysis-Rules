char *
file_open_error_message(int err, gboolean for_writing)
{
  char *errmsg;
  static char errmsg_errno[1024+1];

  switch (err) {

  case WTAP_ERR_NOT_REGULAR_FILE:
    errmsg = "The file \"%s\" is a \"special file\" or socket or other non-regular file.";
    break;

  case WTAP_ERR_FILE_UNKNOWN_FORMAT:
  case WTAP_ERR_UNSUPPORTED:
    /
    errmsg = "The file \"%s\" is not a capture file in a format Ethereal understands.";
    break;

  case WTAP_ERR_UNSUPPORTED_FILE_TYPE:
    /
    errmsg = "Ethereal does not support writing capture files in that format.";
    break;

  case WTAP_ERR_UNSUPPORTED_ENCAP:
  case WTAP_ERR_ENCAP_PER_PACKET_UNSUPPORTED:
    if (for_writing)
      errmsg = "Ethereal cannot save this capture in that format.";
    else
      errmsg = "The file \"%s\" is a capture for a network type that Ethereal doesn't support.";
    break;

  case WTAP_ERR_BAD_RECORD:
    errmsg = "The file \"%s\" appears to be damaged or corrupt.";
    break;

  case WTAP_ERR_CANT_OPEN:
    if (for_writing)
      errmsg = "The file \"%s\" could not be created for some unknown reason.";
    else
      errmsg = "The file \"%s\" could not be opened for some unknown reason.";
    break;

  case WTAP_ERR_SHORT_READ:
    errmsg = "The file \"%s\" appears to have been cut short"
             " in the middle of a packet.";
    break;

  case WTAP_ERR_SHORT_WRITE:
    errmsg = "A full header couldn't be written to the file \"%s\".";
    break;

  case ENOENT:
    if (for_writing)
      errmsg = "The path to the file \"%s\" does not exist.";
    else
      errmsg = "The file \"%s\" does not exist.";
    break;

  case EACCES:
    if (for_writing)
      errmsg = "You do not have permission to create or write to the file \"%s\".";
    else
      errmsg = "You do not have permission to read the file \"%s\".";
    break;

  case EISDIR:
    errmsg = "\"%s\" is a directory (folder), not a file.";
    break;

  default:
    sprintf(errmsg_errno, "The file \"%%s\" could not be opened: %s.",
				wtap_strerror(err));
    errmsg = errmsg_errno;
    break;
  }
  return errmsg;
}
