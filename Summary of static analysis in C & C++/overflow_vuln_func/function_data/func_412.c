gint
packet_mpeg_sect_mjd_to_utc_time(tvbuff_t *tvb, gint offset, nstime_t *utc_time)
{
    gint   bcd_time_offset;     /
    guint8 hour, min, sec;

    if (!utc_time)
        return -1;

    nstime_set_zero(utc_time);
    utc_time->secs  = (tvb_get_ntohs(tvb, offset) - 40587) * 86400;
    bcd_time_offset = offset+2;
    hour            = MPEG_SECT_BCD44_TO_DEC(tvb_get_guint8(tvb, bcd_time_offset));
    min             = MPEG_SECT_BCD44_TO_DEC(tvb_get_guint8(tvb, bcd_time_offset+1));
    sec             = MPEG_SECT_BCD44_TO_DEC(tvb_get_guint8(tvb, bcd_time_offset+2));
    if (hour>23 || min>59 || sec>59)
        return -1;

    utc_time->secs += hour*3600 + min*60 + sec;
    return 5;
}
