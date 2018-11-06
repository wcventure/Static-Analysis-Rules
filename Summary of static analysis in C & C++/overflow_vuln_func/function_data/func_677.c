void randpkt_loop(randpkt_example* example, guint64 produce_count)
{
	guint i;
	int j;
	int err;
	int len_random;
	int len_this_pkt;
	gchar* err_info;
	union wtap_pseudo_header* ps_header;
	guint8 buffer[65536];
	struct wtap_pkthdr* pkthdr;

	pkthdr = g_new0(struct wtap_pkthdr, 1);

	pkthdr->rec_type = REC_TYPE_PACKET;
	pkthdr->presence_flags = WTAP_HAS_TS;
	pkthdr->pkt_encap = example->sample_wtap_encap;

	memset(pkthdr, 0, sizeof(struct wtap_pkthdr));
	memset(buffer, 0, sizeof(buffer));

	ps_header = &pkthdr->pseudo_header;

	/
	if (example->pseudo_buffer)
		memcpy(ps_header, example->pseudo_buffer, example->pseudo_length);

	/
	if (example->sample_buffer)
		memcpy(buffer, example->sample_buffer, example->sample_length);

	/
	for (i = 0; i < produce_count; i++) {
		if (example->produce_max_bytes > 0) {
			len_random = g_rand_int_range(pkt_rand, 0, example->produce_max_bytes + 1);
		}
		else {
			len_random = 0;
		}

		len_this_pkt = example->sample_length + len_random;

		pkthdr->caplen = len_this_pkt;
		pkthdr->len = len_this_pkt;
		pkthdr->ts.secs = i; /

		for (j = example->pseudo_length; j < (int) sizeof(*ps_header); j++) {
			((guint8*)ps_header)[j] = g_rand_int_range(pkt_rand, 0, 0x100);
		}

		for (j = example->sample_length; j < len_this_pkt; j++) {
			/
			if ((int) (100.0*g_rand_double(pkt_rand)) < 3 && j < (len_random - 3)) {
				memcpy(&buffer[j], "%s", 3);
				j += 2;
			} else {
				buffer[j] = g_rand_int_range(pkt_rand, 0, 0x100);
			}
		}

		if (!wtap_dump(example->dump, pkthdr, buffer, &err, &err_info)) {
			fprintf(stderr, "randpkt: Error writing to %s: %s\n",
			    example->filename, wtap_strerror(err));
			switch (err) {

			case WTAP_ERR_UNWRITABLE_ENCAP:
				/
				fprintf(stderr,
				    "Frame has a network type that can't be saved in a \"%s\" file.\n",
				    wtap_file_type_subtype_short_string(WTAP_FILE_TYPE_SUBTYPE_PCAP));
				break;

			case WTAP_ERR_PACKET_TOO_LARGE:
				/
				fprintf(stderr,
					"Frame is too large for a \"%s\" file.\n",
					wtap_file_type_subtype_short_string(WTAP_FILE_TYPE_SUBTYPE_PCAP));
				break;

			case WTAP_ERR_UNWRITABLE_REC_TYPE:
				/
				fprintf(stderr,
					"Record has a record type that can't be saved in a \"%s\" file.\n",
					wtap_file_type_subtype_short_string(WTAP_FILE_TYPE_SUBTYPE_PCAP));
				break;

			case WTAP_ERR_UNWRITABLE_REC_DATA:
				/
				fprintf(stderr,
					"Record has data that can't be saved in a \"%s\" file.\n(%s)\n",
					wtap_file_type_subtype_short_string(WTAP_FILE_TYPE_SUBTYPE_PCAP),
					err_info != NULL ? err_info : "no information supplied");
				g_free(err_info);
				break;

			default:
				break;
			}
		}
	}

	g_free(pkthdr);
}
