static int mxf_read_header(AVFormatContext *s)
{
    MXFContext *mxf = s->priv_data;
    KLVPacket klv;
    int64_t essence_offset = 0;
    int64_t last_pos = -1;
    uint64_t last_pos_index = 1;
    int ret;

    mxf->last_forward_tell = INT64_MAX;
    mxf->edit_units_per_packet = 1;

    if (!mxf_read_sync(s->pb, mxf_header_partition_pack_key, 14)) {
        av_log(s, AV_LOG_ERROR, "could not find header partition pack key\n");
        return AVERROR_INVALIDDATA;
    }
    avio_seek(s->pb, -14, SEEK_CUR);
    mxf->fc = s;
    mxf->run_in = avio_tell(s->pb);

    mxf_read_random_index_pack(s);

    while (!avio_feof(s->pb)) {
        const MXFMetadataReadTableEntry *metadata;
        if (avio_tell(s->pb) == last_pos) {
            av_log(mxf->fc, AV_LOG_ERROR, "MXF structure loop detected\n");
            return AVERROR_INVALIDDATA;
        }
        if ((1ULL<<61) % last_pos_index++ == 0)
            last_pos = avio_tell(s->pb);
        if (klv_read_packet(&klv, s->pb) < 0) {
            /
            if(mxf_parse_handle_partition_or_eof(mxf) <= 0)
                break;
            else
                continue;
        }

        PRINT_KEY(s, "read header", klv.key);
        av_dlog(s, "size %"PRIu64" offset %#"PRIx64"\n", klv.length, klv.offset);
        if (IS_KLV_KEY(klv.key, mxf_encrypted_triplet_key) ||
            IS_KLV_KEY(klv.key, mxf_essence_element_key) ||
            IS_KLV_KEY(klv.key, mxf_avid_essence_element_key) ||
            IS_KLV_KEY(klv.key, mxf_system_item_key)) {

            if (!mxf->current_partition) {
                av_log(mxf->fc, AV_LOG_ERROR, "found essence prior to first PartitionPack\n");
                return AVERROR_INVALIDDATA;
            }

            if (!mxf->current_partition->essence_offset) {
                /
                int64_t op1a_essence_offset =
                    round_to_kag(mxf->current_partition->this_partition +
                                 mxf->current_partition->pack_length,       mxf->current_partition->kag_size) +
                    round_to_kag(mxf->current_partition->header_byte_count, mxf->current_partition->kag_size) +
                    round_to_kag(mxf->current_partition->index_byte_count,  mxf->current_partition->kag_size);

                if (mxf->op == OPAtom) {
                    /
                    mxf->current_partition->essence_offset = avio_tell(s->pb);
                    mxf->current_partition->essence_length = klv.length;
                } else {
                    /
                    mxf->current_partition->essence_offset = op1a_essence_offset;
                }
            }

            if (!essence_offset)
                essence_offset = klv.offset;

            /
            if (mxf_parse_handle_essence(mxf) <= 0)
                break;
            continue;
        } else if (!memcmp(klv.key, mxf_header_partition_pack_key, 13) &&
                   klv.key[13] >= 2 && klv.key[13] <= 4 && mxf->current_partition) {
            /
            if(mxf_parse_handle_partition_or_eof(mxf) <= 0)
                break;
            else if (mxf->parsing_backward)
                continue;
            /
        }

        for (metadata = mxf_metadata_read_table; metadata->read; metadata++) {
            if (IS_KLV_KEY(klv.key, metadata->key)) {
                int res;
                if (klv.key[5] == 0x53) {
                    res = mxf_read_local_tags(mxf, &klv, metadata->read, metadata->ctx_size, metadata->type);
                } else {
                    uint64_t next = avio_tell(s->pb) + klv.length;
                    res = metadata->read(mxf, s->pb, 0, klv.length, klv.key, klv.offset);

                    /
                    if (avio_tell(s->pb) > next) {
                        av_log(s, AV_LOG_ERROR, "read past end of KLV @ %#"PRIx64"\n",
                               klv.offset);
                        return AVERROR_INVALIDDATA;
                    }

                    avio_seek(s->pb, next, SEEK_SET);
                }
                if (res < 0) {
                    av_log(s, AV_LOG_ERROR, "error reading header metadata\n");
                    return res;
                }
                break;
            } else {
                av_log(s, AV_LOG_VERBOSE, "Dark key " PRIxUID "\n",
                       UID_ARG(klv.key));
            }
        }
        if (!metadata->read)
            avio_skip(s->pb, klv.length);
    }
    /
    if (!essence_offset)  {
        av_log(s, AV_LOG_ERROR, "no essence\n");
        return AVERROR_INVALIDDATA;
    }
    avio_seek(s->pb, essence_offset, SEEK_SET);

    mxf_compute_essence_containers(mxf);

    /
    if ((ret = mxf_parse_structural_metadata(mxf)) < 0)
        goto fail;

    if ((ret = mxf_compute_index_tables(mxf)) < 0)
        goto fail;

    if (mxf->nb_index_tables > 1) {
        /
        av_log(mxf->fc, AV_LOG_INFO, "got %i index tables - only the first one (IndexSID %i) will be used\n",
               mxf->nb_index_tables, mxf->index_tables[0].index_sid);
    } else if (mxf->nb_index_tables == 0 && mxf->op == OPAtom) {
        av_log(mxf->fc, AV_LOG_ERROR, "cannot demux OPAtom without an index\n");
        ret = AVERROR_INVALIDDATA;
        goto fail;
    }

    mxf_handle_small_eubc(s);

    return 0;
fail:
    mxf_read_close(s);

    return ret;
}
