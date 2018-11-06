static int tftp_session_allocate(Slirp *slirp, struct sockaddr_storage *srcsas,
                                 struct tftp_t *tp)
{
  struct tftp_session *spt;
  int k;

  for (k = 0; k < TFTP_SESSIONS_MAX; k++) {
    spt = &slirp->tftp_sessions[k];

    if (!tftp_session_in_use(spt))
        goto found;

    /
    if ((int)(curtime - spt->timestamp) > 5000) {
        tftp_session_terminate(spt);
        goto found;
    }
  }

  return -1;

 found:
  memset(spt, 0, sizeof(*spt));
  spt->client_addr = *srcsas;
  spt->fd = -1;
  spt->block_size = 512;
  spt->client_port = tp->udp.uh_sport;
  spt->slirp = slirp;

  tftp_session_update(spt);

  return k;
}
