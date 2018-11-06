struct mbuf *
m_get(Slirp *slirp)
{
	register struct mbuf *m;
	int flags = 0;

	DEBUG_CALL("m_get");

	if (slirp->m_freelist.m_next == &slirp->m_freelist) {
		m = (struct mbuf *)malloc(SLIRP_MSIZE);
		if (m == NULL) goto end_error;
		slirp->mbuf_alloced++;
		if (slirp->mbuf_alloced > MBUF_THRESH)
			flags = M_DOFREE;
		m->slirp = slirp;
	} else {
		m = slirp->m_freelist.m_next;
		remque(m);
	}

	/
	insque(m,&slirp->m_usedlist);
	m->m_flags = (flags | M_USEDLIST);

	/
	m->m_size = SLIRP_MSIZE - sizeof(struct m_hdr);
	m->m_data = m->m_dat;
	m->m_len = 0;
        m->m_nextpkt = NULL;
        m->m_prevpkt = NULL;
end_error:
	DEBUG_ARG("m = %lx", (long )m);
	return m;
}
