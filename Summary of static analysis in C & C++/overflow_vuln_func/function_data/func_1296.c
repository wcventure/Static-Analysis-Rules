static int
iseries_parse_packet (wtap * wth, FILE_T fh,
		      union wtap_pseudo_header *pseudo_header, guint8 * pd,
		      int *err, gchar ** err_info)
{
  gint64 cur_off;
  gboolean isValid, isCurrentPacket, IPread, TCPread, isDATA;
  int num_items_scanned, line, pktline, buflen, i;
  guint32 pkt_len;
  int cap_len, pktnum, month, day, year, hr, min, sec, csec;
  char direction[2], destmac[13], srcmac[13], type[5], ipheader[41],
    tcpheader[81];
  char hex1[17], hex2[17], hex3[17], hex4[17];
  char data[ISERIES_LINE_LENGTH * 2];
  guint8 *buf, *asciibuf, *tcpdatabuf, *workbuf;
  struct tm tm;

  /
  isValid = FALSE;
  for (line = 1; line < ISERIES_PKT_LINES_TO_CHECK; line++)
    {
      cur_off = file_tell (fh);
      if (file_gets (data, ISERIES_LINE_LENGTH, fh) == NULL)
	{
	  *err = file_error (fh);
	  if (*err == 0)
	    {
	      *err = WTAP_ERR_SHORT_READ;
	    }
	  return -1;
	}
      /
      if (wth->capture.iseries->format == ISERIES_FORMAT_UNICODE)
	{
	  iseries_UNICODE_to_ASCII (data, ISERIES_LINE_LENGTH);
	}
      /
      for (i=0; i<8; i++) {      
	if (strncmp(data+i,"*",1) == 0)
	  strncpy(data+i," ",1);
      }
      num_items_scanned =
	sscanf (data,
		"%6d   %1s   %6d  %d:%d:%d.%d               %12s  %12s  ETHV2   Type: %s",
		&pktnum, direction, &cap_len, &hr, &min, &sec, &csec, destmac,
		srcmac, type);
      if (num_items_scanned == 10)
	{
	  /
	  isValid = TRUE;
	  /
	  cap_len += 14;
	  break;
	}
    }

  /
  if (!isValid)
    {
      *err = WTAP_ERR_BAD_RECORD;
      *err_info = g_strdup ("iseries: packet header isn't valid");
      return -1;
    }

  /
  if (wth->capture.iseries->sdate)
    {
      num_items_scanned =
	sscanf (wth->capture.iseries->sdate, "%d/%d/%d", &month, &day, &year);
      tm.tm_year = 100 + year;
      tm.tm_mon = month - 1;
      tm.tm_mday = day;
      tm.tm_hour = hr;
      tm.tm_min = min;
      tm.tm_sec = sec;
      tm.tm_isdst = -1;
      wth->phdr.ts.secs = mktime (&tm);
      /
      if (csec > 99999)
	{
	  wth->phdr.ts.nsecs = csec * 1000;
	}
      /
      else
	{
	  wth->phdr.ts.nsecs = csec * 10000;
	}
      wth->phdr.caplen = cap_len;
      wth->phdr.pkt_encap = WTAP_ENCAP_ETHERNET;
      pseudo_header->eth.fcs_len = -1;
    }

  /
  isCurrentPacket = TRUE;
  IPread = FALSE;
  TCPread = FALSE;
  isDATA = FALSE;
  /
  tcpdatabuf = g_malloc (ISERIES_PKT_ALLOC_SIZE);
  g_snprintf (tcpdatabuf, 1, "%s", "");
  workbuf = g_malloc (ISERIES_PKT_ALLOC_SIZE);
  g_snprintf (workbuf, 1, "%s", "");
  /
  pktline = 0;
  while (isCurrentPacket)
    {
      pktline++;
      /
      if (file_gets (data, ISERIES_LINE_LENGTH, fh) == NULL)
	{
	  if (file_eof (fh))
	    {
	      break;
	    }
	  else
	    {
	      *err = file_error (fh);
	      if (*err == 0)
		{
		  *err = WTAP_ERR_SHORT_READ;
		}
	      return -1;
	    }
	}

      /
      if (wth->capture.iseries->format == ISERIES_FORMAT_UNICODE)
	{
	  buflen = iseries_UNICODE_to_ASCII (data, ISERIES_LINE_LENGTH);
	}
      else
	{
	  /
	  buflen = strlen (data);
	}

      /
      num_items_scanned = sscanf (data + 22, "IP Header  : %40s", ipheader);
      if (num_items_scanned == 1)
	{
	  IPread = TRUE;
	}

      /
      num_items_scanned = sscanf (data + 22, "TCP Header : %80s", tcpheader);
      if (num_items_scanned == 1)
	{
	  TCPread = TRUE;
	}

      /
      num_items_scanned =
	sscanf (data + 27, "%16[A-Z0-9] %16[A-Z0-9] %16[A-Z0-9] %16[A-Z0-9]",
		hex1, hex2, hex3, hex4);
      if (num_items_scanned > 0)
	{
	  isDATA = TRUE;
	  /
	  switch (num_items_scanned)
	    {
	    case 1:
	      g_snprintf (workbuf, ISERIES_PKT_ALLOC_SIZE, "%s%s", tcpdatabuf,
			  hex1);
	      break;
	    case 2:
	      g_snprintf (workbuf, ISERIES_PKT_ALLOC_SIZE, "%s%s%s",
			  tcpdatabuf, hex1, hex2);
	      break;
	    case 3:
	      g_snprintf (workbuf, ISERIES_PKT_ALLOC_SIZE, "%s%s%s%s",
			  tcpdatabuf, hex1, hex2, hex3);
	      break;
	    default:
	      g_snprintf (workbuf, ISERIES_PKT_ALLOC_SIZE, "%s%s%s%s%s",
			  tcpdatabuf, hex1, hex2, hex3, hex4);
	    }
	  memcpy (tcpdatabuf, workbuf, ISERIES_PKT_ALLOC_SIZE);
	}

      /
      if ((strncmp (data + 80, ISERIES_PKT_MAGIC_STR, ISERIES_PKT_MAGIC_LEN)
	   == 0) && pktline > 1)
	{
	  isCurrentPacket = FALSE;
	  cur_off = file_tell (fh);
	  if (cur_off == -1)
	    {
	      /
	      *err = file_error (fh);
	      return -1;
	    }
	  if (file_seek (fh, cur_off - buflen, SEEK_SET, err) == -1)
	    {
	      return -1;
	    }
	}
    }

  /
  if (wth->capture.iseries->tcp_formatted)
    {
      if (!IPread)
	{
	  *err = WTAP_ERR_BAD_RECORD;
	  *err_info = g_strdup ("iseries: IP header isn't valid");
	  return -1;
	}
      if (!TCPread)
	{
	  *err = WTAP_ERR_BAD_RECORD;
	  *err_info = g_strdup ("iseries: TCP header isn't valid");
	  return -1;
	}
    }

  /
  asciibuf = g_malloc (ISERIES_PKT_ALLOC_SIZE);
  if (isDATA)
    {
      /
      if (wth->capture.iseries->tcp_formatted)
	{
	  /
	  g_snprintf (asciibuf, ISERIES_PKT_ALLOC_SIZE, "%s%s%s%s%s%s",
		      destmac, srcmac, type, ipheader, tcpheader, tcpdatabuf);
	}
      else
	{
	  /
	  g_snprintf (asciibuf, ISERIES_PKT_ALLOC_SIZE, "%s%s%s%s", destmac,
		      srcmac, type, tcpdatabuf);
	}
    }
  else
    {
      /
      g_snprintf (asciibuf, ISERIES_PKT_ALLOC_SIZE, "%s%s%s%s%s", destmac,
		  srcmac, type, ipheader, tcpheader);
    }

  /
  num_items_scanned = sscanf (asciibuf + 32, "%4x", &pkt_len);
  wth->phdr.len = pkt_len + 14;
  if (wth->phdr.caplen > wth->phdr.len)
    wth->phdr.len = wth->phdr.caplen;

  /
  if (pd == NULL)
    {
      buffer_assure_space (wth->frame_buffer, ISERIES_MAX_PACKET_LEN);
      buf = buffer_start_ptr (wth->frame_buffer);
      /
      iseries_parse_hex_string (asciibuf, buf, strlen (asciibuf));
    }
  else
    {
      /
      iseries_parse_hex_string (asciibuf, pd, strlen (asciibuf));
    }

  /
  *err = 0;
  free (asciibuf);
  free (tcpdatabuf);
  free (workbuf);
  return wth->phdr.len;
}
