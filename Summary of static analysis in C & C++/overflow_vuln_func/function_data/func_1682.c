static int dissect_DIS_FIELDS_CLOCK_TIME(tvbuff_t *tvb, proto_tree *tree, int offset, const char* clock_name)
{
    proto_item  *ti;
    proto_tree  *sub_tree;
    /
    static guint MSEC_PER_HOUR = 60 * 60 * 1000;
    static guint FSV = 0x7fffffff;
    guint32 hour, uintVal;
    guint64 ms;
    guint isAbsolute = 0;
    nstime_t tv;

    sub_tree = proto_tree_add_subtree(tree, tvb, offset, 8, ett_clock_time, NULL, clock_name);

    hour = tvb_get_ntohl(tvb, offset);
    uintVal = tvb_get_ntohl(tvb, offset+4);

    /
    isAbsolute = uintVal & 1;

    /
    ms = (uintVal >> 1) * MSEC_PER_HOUR / FSV;

    tv.secs = (time_t)ms/1000;
    tv.nsecs = (int)(ms%1000)*1000000;

    /
    tv.secs += (hour*3600);

    ti = proto_tree_add_time(sub_tree, hf_dis_clocktime, tvb, offset, 8, &tv);
    if (isAbsolute)
    {
        proto_item_append_text(ti, " (absolute)");
    }
    else
    {
        proto_item_append_text(ti, " (relative)");
    }

   return (offset+8);
}
