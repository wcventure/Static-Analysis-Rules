static const char *
situation2str(guint32 type) {

#define SIT_MSG_NUM	1024
#define SIT_IDENTITY	0x01
#define SIT_SECRECY	0x02
#define SIT_INTEGRITY	0x04

  static char	msg[SIT_MSG_NUM];
  int		n = 0;
  char *	sep = "";
  
  if (type & SIT_IDENTITY) {
    n += snprintf(msg, SIT_MSG_NUM-n, "%sIDENTITY", sep);
    sep = " & ";
  }
  if (type & SIT_SECRECY) {
    n += snprintf(msg, SIT_MSG_NUM-n, "%sSECRECY", sep);
    sep = " & ";
  }
  if (type & SIT_INTEGRITY) {
    n += snprintf(msg, SIT_MSG_NUM-n, "%sINTEGRITY", sep);
    sep = " & ";
  }

  return msg;
}
