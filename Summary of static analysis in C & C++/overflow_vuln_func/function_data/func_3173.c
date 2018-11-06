static int rtp_asf_fix_header(uint8_t *buf, int len)
{
    uint8_t *p = buf, *end = buf + len;

    if (len < sizeof(ff_asf_guid) * 2 + 22 ||
        memcmp(p, ff_asf_header, sizeof(ff_asf_guid))) {
        return -1;
    }
    p += sizeof(ff_asf_guid) + 14;
    do {
        uint64_t chunksize = AV_RL64(p + sizeof(ff_asf_guid));
        if (memcmp(p, ff_asf_file_header, sizeof(ff_asf_guid))) {
            if (chunksize > end - p)
                return -1;
            p += chunksize;
            continue;
        }

        /
        p += 6 * 8 + 3 * 4 + sizeof(ff_asf_guid) * 2;
        if (p + 8 <= end && AV_RL32(p) == AV_RL32(p + 4)) {
            /
            AV_WL32(p, 0);
            return 0;
        }
        break;
    } while (end - p >= sizeof(ff_asf_guid) + 8);

    return -1;
}
