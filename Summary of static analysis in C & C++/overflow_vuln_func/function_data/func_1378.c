static int
process_data(pppdump_t *state, FILE_T fh, pkt_t *pkt, int n, guint8 *pd, int *err,
		gboolean *state_saved)
{
	int	c;
	int	num_bytes = n;
	int	num_written;

	*state_saved = FALSE;
	for (; num_bytes > 0; --num_bytes) {
		c = file_getc(fh);
		state->offset++;
		switch (c) {
			case EOF:
				if (*err == 0) {
					*err = WTAP_ERR_SHORT_READ;
				}
				return -1;
				break;

			case '~':
				if (pkt->cnt > 0) {
					pkt->esc = FALSE;

					num_written = pkt->cnt - 2;
					pkt->cnt = 0;
					if (num_written <= 0) {
						return 0;
					}

					if (num_written > sizeof(pd)) {
						*err = WTAP_ERR_UNC_OVERFLOW;
						return -1;
					}

					memcpy(pd, pkt->buf, num_written);

					num_bytes--;
					if (num_bytes > 0) {
						if (!save_prec_state(state, num_bytes, pkt)) {
							*err = errno;
							return -1;
						}
						*state_saved = TRUE;
					}
					return num_written;
				}
				break;

			case '}':
				if (!pkt->esc) {
					pkt->esc = TRUE;
					break;
				}
				/

			default:
				if (pkt->esc) {
					c ^= 0x20;
					pkt->esc = FALSE;
				}
		
				pkt->buf[pkt->cnt++] = c;
				break;
		}
	}

	/
	return 0;

}
