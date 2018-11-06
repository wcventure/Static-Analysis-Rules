void
capture_prism(const guchar *pd, int offset, int len, packet_counts *ld)
{
    if(!BYTES_ARE_IN_FRAME(offset, len, (int)sizeof(struct prism_hdr))) {
        ld->other ++;
        return;
    }
    offset += sizeof(struct prism_hdr);

    /
    capture_ieee80211(pd, offset, len, ld);
}
