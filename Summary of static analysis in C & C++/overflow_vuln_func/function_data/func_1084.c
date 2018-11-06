static gboolean
iseries_parse_packet (wtap * wth, FILE_T fh, struct wtap_pkthdr *phdr,
                      Buffer *buf, int *err, gchar **err_info)
{
  iseries_t *iseries = (iseries_t *)wth->priv;
  gint64     cur_off;
  gboolean   isValid, isCurrentPacket;
  int        num_items_scanned, line, pktline, buflen;
  int        pkt_len, pktnum, hr, min, sec;
  char       direction[2], destmac[13], srcmac[13], type[5], csec[9+1];
  char       data[ISERIES_LINE_LENGTH * 2];
  int        offset;
  char      *ascii_buf;
  int        ascii_offset;
  struct tm  tm;

  /
  isValid = FALSE;
  for (line = 1; line < ISERIES_PKT_LINES_TO_CHECK; line++)
    {
      if (file_gets (data, ISERIES_LINE_LENGTH, fh) == NULL)
        {
          *err = file_error (fh, err_info);
          return FALSE;
        }
      /
      if (iseries->format == ISERIES_FORMAT_UNICODE)
        {
         iseries_UNICODE_to_ASCII ((guint8 *)data, ISERIES_LINE_LENGTH);
        }
      ascii_strup_inplace (data);
      num_items_scanned =
        sscanf (data,
                "%*[ \n\t]%6d%*[ *\n\t]%1s%*[ \n\t]%6d%*[ \n\t]%2d:%2d:%2d.%9[0-9]%*[ \n\t]"
                "%12s%*[ \n\t]%12s%*[ \n\t]ETHV2%*[ \n\t]TYPE:%*[ \n\t]%4s",
                &pktnum, direction, &pkt_len, &hr, &min, &sec, csec, destmac,
                srcmac, type);
      if (num_items_scanned == 10)
        {
          /
          isValid = TRUE;
          /
          pkt_len += 14;
          break;
        }
    }

  /
  if (!isValid)
    {
      *err = WTAP_ERR_BAD_FILE;
      *err_info = g_strdup ("iseries: packet header isn't valid");
      return FALSE;
    }

  phdr->rec_type = REC_TYPE_PACKET;
  phdr->presence_flags = WTAP_HAS_CAP_LEN;

  /
  if (iseries->have_date)
    {
      phdr->presence_flags |= WTAP_HAS_TS;
      tm.tm_year        = 100 + iseries->year;
      tm.tm_mon         = iseries->month - 1;
      tm.tm_mday        = iseries->day;
      tm.tm_hour        = hr;
      tm.tm_min         = min;
      tm.tm_sec         = sec;
      tm.tm_isdst       = -1;
      phdr->ts.secs = mktime (&tm);
      csec[sizeof(csec) - 1] = '\0';
      switch (strlen(csec))
        {
          case 0:
            phdr->ts.nsecs = 0;
            break;
          case 1:
            phdr->ts.nsecs = atoi(csec) * 100000000;
            break;
          case 2:
            phdr->ts.nsecs = atoi(csec) * 10000000;
            break;
          case 3:
            phdr->ts.nsecs = atoi(csec) * 1000000;
            break;
          case 4:
            phdr->ts.nsecs = atoi(csec) * 100000;
            break;
          case 5:
            phdr->ts.nsecs = atoi(csec) * 10000;
            break;
          case 6:
            phdr->ts.nsecs = atoi(csec) * 1000;
            break;
          case 7:
            phdr->ts.nsecs = atoi(csec) * 100;
            break;
          case 8:
            phdr->ts.nsecs = atoi(csec) * 10;
            break;
          case 9:
            phdr->ts.nsecs = atoi(csec);
            break;
        }
    }

  phdr->len                       = pkt_len;
  phdr->pkt_encap                 = WTAP_ENCAP_ETHERNET;
  phdr->pseudo_header.eth.fcs_len = -1;

  ascii_buf = (char *)g_malloc (ISERIES_PKT_ALLOC_SIZE);
  g_snprintf(ascii_buf, ISERIES_PKT_ALLOC_SIZE, "%s%s%s", destmac, srcmac, type);
  ascii_offset = 14*2; /

  /
  isCurrentPacket = TRUE;

  /
  pktline = 0;
  while (isCurrentPacket)
    {
      pktline++;
      /
      if (file_gets (data, ISERIES_LINE_LENGTH, fh) == NULL)
        {
          *err = file_error (fh, err_info);
          if (*err == 0)
            {
              /
              break;
            }
          goto errxit;
        }

      /
      if (iseries->format == ISERIES_FORMAT_UNICODE)
        {
         buflen = iseries_UNICODE_to_ASCII ((guint8 *)data, ISERIES_LINE_LENGTH);
        }
      else
        {
          /
          buflen = (int) strlen (data);
        }

      /
      for (offset = 0; g_ascii_isspace(data[offset]); offset++)
        ;

      /
      if (offset == 22)
        {
          if (strncmp(data + 22, "IP Header  :  ", 14) == 0 ||
              strncmp(data + 22, "IPv6 Header:  ", 14) == 0 ||
              strncmp(data + 22, "ARP Header :  ", 14) == 0 ||
              strncmp(data + 22, "TCP Header :  ", 14) == 0 ||
              strncmp(data + 22, "UDP Header :  ", 14) == 0 ||
              strncmp(data + 22, "ICMP Header:  ", 14) == 0 ||
              strncmp(data + 22, "ICMPv6  Hdr:  ", 14) == 0 ||
              strncmp(data + 22, "Option  Hdr:  ", 14) == 0)
            {
              ascii_offset = append_hex_digits(ascii_buf, ascii_offset,
                                               ISERIES_PKT_ALLOC_SIZE - 1,
                                               data + 22 + 14, err,
                                               err_info);
              if (ascii_offset == -1)
                {
                  /
                  return FALSE;
                }
              continue;
            }
        }

      /
      if (offset == 9)
        {
          if (strncmp(data + 9, "Data . . . . . :  ", 18) == 0)
            {
              ascii_offset = append_hex_digits(ascii_buf, ascii_offset,
                                               ISERIES_PKT_ALLOC_SIZE - 1,
                                               data + 9 + 18, err,
                                               err_info);
              if (ascii_offset == -1)
                {
                  /
                  return FALSE;
                }
              continue;
            }
        }

      /
      if (offset == 36 || offset == 27)
        {
          ascii_offset = append_hex_digits(ascii_buf, ascii_offset,
                                           ISERIES_PKT_ALLOC_SIZE - 1,
                                           data + offset, err,
                                           err_info);
          if (ascii_offset == -1)
            {
              /
              return FALSE;
            }
          continue;
        }

      /
      ascii_strup_inplace (data);
      /
      num_items_scanned =
          sscanf (data+78,
          "%*[ \n\t]ETHV2%*[ .:\n\t]TYPE%*[ .:\n\t]%4s",type);
      if ((num_items_scanned == 1) && pktline > 1)
        {
          isCurrentPacket = FALSE;
          cur_off = file_tell( fh);
          if (cur_off == -1)
            {
              /
              *err = file_error (fh, err_info);
              goto errxit;
            }
          if (file_seek (fh, cur_off - buflen, SEEK_SET, err) == -1)
            {
              /
              goto errxit;
            }
        }
    }
  ascii_buf[ascii_offset] = '\0';

  /
  phdr->caplen = ((guint32) strlen (ascii_buf))/2;

  /
  ws_buffer_assure_space (buf, ISERIES_MAX_PACKET_LEN);
  /
  iseries_parse_hex_string (ascii_buf, ws_buffer_start_ptr (buf), strlen (ascii_buf));

  /
  *err = 0;
  g_free (ascii_buf);
  return TRUE;

errxit:
  g_free (ascii_buf);
  return FALSE;
}
