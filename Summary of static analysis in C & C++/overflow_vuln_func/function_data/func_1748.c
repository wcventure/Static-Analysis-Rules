void
address_to_str_buf(const address *addr, gchar *buf, int buf_len)
{
  struct atalk_ddp_addr ddp_addr;

  switch(addr->type){
  case AT_NONE:
    g_snprintf(buf, buf_len, "%s", "");
    break;
  case AT_ETHER:
    g_snprintf(buf, buf_len, "%02x:%02x:%02x:%02x:%02x:%02x", addr->data[0], addr->data[1], addr->data[2], addr->data[3], addr->data[4], addr->data[5]);
    break;
  case AT_IPv4:
    ip_to_str_buf(addr->data, buf);
    break;
  case AT_IPv6:
    inet_ntop(AF_INET6, addr->data, buf, INET6_ADDRSTRLEN);
    break;
  case AT_IPX:
    g_snprintf(buf, buf_len, "%02x%02x%02x%02x.%02x%02x%02x%02x%02x%02x", addr->data[0], addr->data[1], addr->data[2], addr->data[3], addr->data[4], addr->data[5], addr->data[6], addr->data[7], addr->data[8], addr->data[9]);
    break;
  case AT_SNA:
    sna_fid_to_str_buf(addr, buf, buf_len);
    break;
  case AT_ATALK:
    memcpy(&ddp_addr, addr->data, sizeof ddp_addr);
    atalk_addr_to_str_buf(&ddp_addr, buf, buf_len);
    break;
  case AT_VINES:
    vines_addr_to_str_buf(addr->data, buf, buf_len);
    break;
  case AT_USB:
    usb_addr_to_str_buf(addr->data, buf, buf_len);
    break;
  case AT_OSI:
    print_nsap_net_buf(addr->data, addr->len, buf, buf_len);
    break;
  case AT_ARCNET:
    g_snprintf(buf, buf_len, "0x%02X", addr->data[0]);
    break;
  case AT_FC:
    g_snprintf(buf, buf_len, "%02x.%02x.%02x", addr->data[0], addr->data[1], addr->data[2]);
    break;
  case AT_SS7PC:
    mtp3_addr_to_str_buf(addr->data, buf, buf_len);
    break;
  case AT_STRINGZ:
    g_snprintf(buf, buf_len, "%s", addr->data);
    break;
  case AT_EUI64:
    g_snprintf(buf, buf_len, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
            addr->data[0], addr->data[1], addr->data[2], addr->data[3],
            addr->data[4], addr->data[5], addr->data[6], addr->data[7]);
    break;
  case AT_URI: {
    int copy_len = addr->len < (buf_len - 1) ? addr->len : (buf_len - 1);
    memmove(buf, addr->data, copy_len );
    buf[copy_len] = '\0';
    }
    break;
  case AT_TIPC:
	  tipc_addr_to_str_buf(addr->data, buf, buf_len);
	  break;
  default:
    g_assert_not_reached();
  }
}
