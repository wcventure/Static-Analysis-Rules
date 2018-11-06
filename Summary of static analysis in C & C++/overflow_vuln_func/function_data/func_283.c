static int
decode_MPLS_stack(tvbuff_t *tvb, gint offset, char *buf, int buflen)
{
    guint32     label_entry;    /
    gint        index;          /

    index = offset ;
    label_entry = 0x000000 ;

    buf[0] = '\0' ;

    while ((label_entry & 0x000001) == 0) {

        label_entry = tvb_get_ntoh24(tvb, index) ;

        /
        if((index-offset)==0&&(label_entry==0||label_entry==0x800000)) {
            snprintf(buf, buflen, "0 (withdrawn)");
            return (1);
        }
        snprintf(buf, buflen,"%s%u%s", buf, (label_entry >> 4), ((label_entry & 0x000001) == 0) ? "," : " (bottom)");
        index += 3 ;
    }

    return((index - offset) / 3);
}
