static gboolean
iseries_check_file_type (wtap * wth, int *err, int format)
{
  guint line;
  int num_items_scanned;
  char buf[ISERIES_LINE_LENGTH], protocol[8], tcpformat[1];
  guint8 *sdate;

  /
  sdate = g_malloc (10);
  wth->capture.iseries = g_malloc (sizeof (iseries_t));
  wth->capture.iseries->sdate = NULL;
  wth->capture.iseries->format = format;
  wth->capture.iseries->tcp_formatted = FALSE;

  for (line = 0; line < ISERIES_HDR_LINES_TO_CHECK; line++)
    {
      if (file_gets (buf, ISERIES_LINE_LENGTH, wth->fh) != NULL)
	{
	  /
	  if (wth->capture.iseries->format == ISERIES_FORMAT_UNICODE)
	    {
	      iseries_UNICODE_to_ASCII (buf, ISERIES_LINE_LENGTH);
	    }
	  num_items_scanned = sscanf (buf,
				      "   Object protocol  . . . . . . :  %8s",
				      protocol);
	  if (num_items_scanned == 1)
	    {
	      if (memcmp (protocol, "ETHERNET", 8) != 0)
		return FALSE;
	    }

	  /
	  num_items_scanned = sscanf (buf,
				      "   Format TCP/IP data only  . . :  %1s",
				      tcpformat);
	  if (num_items_scanned == 1)
	    {
	      if (strncmp (tcpformat, "Y", 1) == 0)
		{
		  wth->capture.iseries->tcp_formatted = TRUE;
		}
	      else
		{
		  wth->capture.iseries->tcp_formatted = FALSE;
		}
	    }

	  /
	  num_items_scanned = sscanf (buf,
				      "   Start date/Time  . . . . . . :  %8s",
				      sdate);
	  if (num_items_scanned == 1)
	    {
	      wth->capture.iseries->sdate = sdate;
	    }
	}
      else
	{
	  /
	  if (file_eof (wth->fh))
	    *err = 0;
	  else
	    *err = file_error (wth->fh);
	  return FALSE;
	}
    }
  *err = 0;
  return TRUE;
}
