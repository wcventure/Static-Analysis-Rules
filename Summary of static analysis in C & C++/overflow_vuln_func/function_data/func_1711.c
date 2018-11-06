static gchar *host_name_lookup(guint addr, gboolean *found)
{
  int hash_idx;
  hashipv4_t * volatile tp;
  struct hostent *hostp;
#ifdef HAVE_GNU_ADNS
  adns_queue_msg_t *qmsg;
#endif

  *found = TRUE;

  hash_idx = HASH_IPV4_ADDRESS(addr);

  tp = ipv4_table[hash_idx];

  if( tp == NULL ) {
    tp = ipv4_table[hash_idx] = (hashipv4_t *)g_malloc(sizeof(hashipv4_t));
  } else {
    while(1) {
      if( tp->addr == addr ) {
	if (tp->is_dummy_entry)
	  *found = FALSE;
	return tp->name;
      }
      if (tp->next == NULL) {
	tp->next = (hashipv4_t *)g_malloc(sizeof(hashipv4_t));
	tp = tp->next;
	break;
      }
      tp = tp->next;
    }
  }

  /
  tp->addr = addr;
  tp->next = NULL;

#ifdef HAVE_GNU_ADNS
  if ((g_resolv_flags & RESOLV_CONCURRENT) &&
      prefs.name_resolve_concurrency > 0 &&
      gnu_adns_initialized) {
    qmsg = g_malloc(sizeof(adns_queue_msg_t));
    qmsg->type = AF_INET;
    qmsg->ip4_addr = addr;
    qmsg->submitted = FALSE;
    adns_queue_head = g_list_append(adns_queue_head, (gpointer) qmsg);

    tp->is_dummy_entry = TRUE;
    ip_to_str_buf((guint8 *)&addr, tp->name);
    return tp->name;
  }
#endif /

  /
  if (addr != 0 && (g_resolv_flags & RESOLV_NETWORK)) {
  /

# ifdef AVOID_DNS_TIMEOUT

    /

    if (!setjmp(hostname_env)) {
      signal(SIGALRM, abort_network_query);
      alarm(DNS_TIMEOUT);
# endif /

      hostp = gethostbyaddr((char *)&addr, 4, AF_INET);

# ifdef AVOID_DNS_TIMEOUT
      alarm(0);
# endif /

      if (hostp != NULL) {
	strncpy(tp->name, hostp->h_name, MAXNAMELEN);
	tp->name[MAXNAMELEN-1] = '\0';
	tp->is_dummy_entry = FALSE;
	return tp->name;
      }

# ifdef AVOID_DNS_TIMEOUT
    }
# endif /

  }

  /

  ip_to_str_buf((guint8 *)&addr, tp->name);
  tp->is_dummy_entry = TRUE;
  *found = FALSE;

  return (tp->name);

} /
