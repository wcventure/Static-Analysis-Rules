static int mxf_read_header(AVFormatContext *s, AVFormatParameters *ap)
{
    MXFContext *mxf = s->priv_data;
    KLVPacket klv;
    int64_t essence_offset = 0;

    mxf->last_forward_tell = INT64_MAX;

    if (!mxf_read_sync(s->pb, mxf_header_partition_pack_key, 14)) {
        av_log(s, AV_LOG_ERROR, "could not find header partition pack key\n");
        return AVERROR_INVALIDDATA;
    }
    avio_seek(s->pb, -14, SEEK_CUR);
    mxf->fc = s;
    mxf->run_in = avio_tell(s->pb);

    while (!s->pb->eof_reached) {
        const MXFMetadataReadTableEntry *metadata;

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
            if (!mxf->current_partition->essence_offset) {
                compute_partition_essence_offset(s, mxf, &klv);
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
        }

        for (metadata = mxf_metadata_read_table; metadata->read; metadata++) {
            if (IS_KLV_KEY(klv.key, metadata->key)) {
                int res;
                if (klv.key[5] == 0x53) {
                    res = mxf_read_local_tags(mxf, &klv, metadata->read, metadata->ctx_size, metadata->type);
                } else {
                    uint64_t next = avio_tell(s->pb) + klv.length;
                    res = metadata->read(mxf, s->pb, 0, klv.length, klv.key, klv.offset);
                    avio_seek(s->pb, next, SEEK_SET);
                }
                if (res < 0) {
                    av_log(s, AV_LOG_ERROR, "error reading header metadata\n");
                    return res;
                }
                break;
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

    return mxf_parse_structural_metadata(mxf);
}
