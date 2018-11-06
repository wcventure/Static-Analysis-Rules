static gboolean
pcapng_write_name_resolution_block(wtap_dumper *wdh, int *err)
{
    pcapng_block_header_t bh;
    pcapng_name_resolution_block_t nrb;
    guint8 *rec_data;
    gint rec_off, namelen, tot_rec_len;
    hashipv4_t *ipv4_hash_list_entry;
    hashipv6_t *ipv6_hash_list_entry;
    int i;

    if ((!wdh->addrinfo_lists) || ((!wdh->addrinfo_lists->ipv4_addr_list)&&(!wdh->addrinfo_lists->ipv6_addr_list))) {
        return TRUE;
    }

    rec_off = 8; /
    bh.block_type = BLOCK_TYPE_NRB;
    bh.block_total_length = rec_off + 8; /
    rec_data = (guint8 *)g_malloc(NRES_REC_MAX_SIZE);

    if (wdh->addrinfo_lists->ipv4_addr_list){
        i = 0;
        ipv4_hash_list_entry = (hashipv4_t *)g_list_nth_data(wdh->addrinfo_lists->ipv4_addr_list, i);
        while(ipv4_hash_list_entry != NULL){

            nrb.record_type = NRES_IP4RECORD;
            namelen = (gint)strlen(ipv4_hash_list_entry->name) + 1;
            nrb.record_len = 4 + namelen;
            tot_rec_len = 4 + nrb.record_len + PADDING4(nrb.record_len);

            if (rec_off + tot_rec_len > NRES_REC_MAX_SIZE){
                /
                memcpy(rec_data, &bh, sizeof(bh));

                /
                memset(rec_data + rec_off, 0, 4);
                rec_off += 4;

                memcpy(rec_data + rec_off, &bh.block_total_length, sizeof(bh.block_total_length));

                pcapng_debug2("pcapng_write_name_resolution_block: Write bh.block_total_length bytes %d, rec_off %u", bh.block_total_length, rec_off);

                if (!wtap_dump_file_write(wdh, rec_data, bh.block_total_length, err)) {
                    g_free(rec_data);
                    return FALSE;
                }
                wdh->bytes_dumped += bh.block_total_length;

                /
                rec_off = 8; /
                bh.block_type = BLOCK_TYPE_NRB;
                bh.block_total_length = rec_off + 8; /

            }

            bh.block_total_length += tot_rec_len;
            memcpy(rec_data + rec_off, &nrb, sizeof(nrb));
            rec_off += 4;
            memcpy(rec_data + rec_off, &(ipv4_hash_list_entry->addr), 4);
            rec_off += 4;
            memcpy(rec_data + rec_off, ipv4_hash_list_entry->name, namelen);
            rec_off += namelen;
            memset(rec_data + rec_off, 0, PADDING4(namelen));
            rec_off += PADDING4(namelen);
            pcapng_debug1("NRB: added IPv4 record for %s", ipv4_hash_list_entry->name);

            i++;
            ipv4_hash_list_entry = (hashipv4_t *)g_list_nth_data(wdh->addrinfo_lists->ipv4_addr_list, i);
        }
        g_list_free(wdh->addrinfo_lists->ipv4_addr_list);
        wdh->addrinfo_lists->ipv4_addr_list = NULL;
    }

    if (wdh->addrinfo_lists->ipv6_addr_list){
        i = 0;
        ipv6_hash_list_entry = (hashipv6_t *)g_list_nth_data(wdh->addrinfo_lists->ipv6_addr_list, i);
        while(ipv6_hash_list_entry != NULL){

            nrb.record_type = NRES_IP6RECORD;
            namelen = (gint)strlen(ipv6_hash_list_entry->name) + 1;
            nrb.record_len = 16 + namelen;  /
            /
            tot_rec_len = 4 + nrb.record_len + PADDING4(nrb.record_len);

            if (rec_off + tot_rec_len > NRES_REC_MAX_SIZE){
                /
                memcpy(rec_data, &bh, sizeof(bh));

                /
                memset(rec_data + rec_off, 0, 4);
                rec_off += 4;

                memcpy(rec_data + rec_off, &bh.block_total_length, sizeof(bh.block_total_length));

                pcapng_debug2("pcapng_write_name_resolution_block: Write bh.block_total_length bytes %d, rec_off %u", bh.block_total_length, rec_off);

                if (!wtap_dump_file_write(wdh, rec_data, bh.block_total_length, err)) {
                    g_free(rec_data);
                    return FALSE;
                }
                wdh->bytes_dumped += bh.block_total_length;

                /
                rec_off = 8; /
                bh.block_type = BLOCK_TYPE_NRB;
                bh.block_total_length = rec_off + 8; /

            }

            bh.block_total_length += tot_rec_len;
            memcpy(rec_data + rec_off, &nrb, sizeof(nrb));
            rec_off += 4;
            memcpy(rec_data + rec_off, &(ipv6_hash_list_entry->addr), 16);
            rec_off += 16;
            memcpy(rec_data + rec_off, ipv6_hash_list_entry->name, namelen);
            rec_off += namelen;
            memset(rec_data + rec_off, 0, PADDING4(namelen));
            rec_off += PADDING4(namelen);
            pcapng_debug1("NRB: added IPv6 record for %s", ipv6_hash_list_entry->name);

            i++;
            ipv6_hash_list_entry = (hashipv6_t *)g_list_nth_data(wdh->addrinfo_lists->ipv6_addr_list, i);
        }
        g_list_free(wdh->addrinfo_lists->ipv6_addr_list);
        wdh->addrinfo_lists->ipv6_addr_list = NULL;
    }

    /
    if (wdh->nrb_hdr) {
        gboolean have_options = FALSE;
        guint32 options_total_length = 0;
        struct option option_hdr;
        guint32 comment_len = 0, comment_pad_len = 0;
        wtapng_name_res_t *nrb_hdr = wdh->nrb_hdr;
        gint prev_rec_off = rec_off;

        /
        if (nrb_hdr->opt_comment) {
            have_options = TRUE;
            comment_len = (guint32)strlen(nrb_hdr->opt_comment) & 0xffff;
            if ((comment_len % 4)) {
                comment_pad_len = 4 - (comment_len % 4);
            } else {
                comment_pad_len = 0;
            }
            options_total_length = options_total_length + comment_len + comment_pad_len + 4 / ;
        }

        if (have_options) {
            /
            options_total_length += 4;

            if (rec_off + options_total_length > NRES_REC_MAX_SIZE) {
                /
                memcpy(rec_data, &bh, sizeof(bh));

                /
                memset(rec_data + rec_off, 0, 4);
                rec_off += 4;

                memcpy(rec_data + rec_off, &bh.block_total_length, sizeof(bh.block_total_length));

                pcapng_debug2("pcapng_write_name_resolution_block: Write bh.block_total_length bytes %d, rec_off %u", bh.block_total_length, rec_off);

                if (!wtap_dump_file_write(wdh, rec_data, bh.block_total_length, err)) {
                    g_free(rec_data);
                    return FALSE;
                }
                wdh->bytes_dumped += bh.block_total_length;

                /
                prev_rec_off = rec_off = 8; /
                bh.block_type = BLOCK_TYPE_NRB;
                bh.block_total_length = rec_off + 8; /
            }

            bh.block_total_length += options_total_length;

            if (comment_len > 0) {
                option_hdr.type         = OPT_COMMENT;
                option_hdr.value_length = comment_len;

                memcpy(rec_data + rec_off, &option_hdr, sizeof(option_hdr));
                rec_off += (gint)sizeof(option_hdr);

                /
                memcpy(rec_data + rec_off, nrb_hdr->opt_comment, comment_len);
                rec_off += comment_len;
                memset(rec_data + rec_off, 0, comment_pad_len);
                rec_off += comment_pad_len;

                pcapng_debug2("pcapng_write_name_resolution_block: Wrote Options comments: comment_len %u, comment_pad_len %u",
                              comment_len,
                              comment_pad_len);
            }

            /
            memset(rec_data + rec_off, 0, 4);
            rec_off += 4;

            /
            g_assert((gint)options_total_length == rec_off - prev_rec_off);
        }
    }

    /
    memcpy(rec_data, &bh, sizeof(bh));

    /
    memset(rec_data + rec_off, 0, 4);
    rec_off += 4;

    memcpy(rec_data + rec_off, &bh.block_total_length, sizeof(bh.block_total_length));

    pcapng_debug2("pcapng_write_name_resolution_block: Write bh.block_total_length bytes %d, rec_off %u", bh.block_total_length, rec_off);

    if (!wtap_dump_file_write(wdh, rec_data, bh.block_total_length, err)) {
        g_free(rec_data);
        return FALSE;
    }

    g_free(rec_data);
    wdh->bytes_dumped += bh.block_total_length;
    return TRUE;
}
