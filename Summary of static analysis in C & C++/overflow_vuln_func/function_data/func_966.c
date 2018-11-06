static void
dissect_zcl_msg_get_cancel(tvbuff_t *tvb, proto_tree *tree, guint *offset)
{
    nstime_t impl_time;

    /
    impl_time.secs = tvb_get_letohl(tvb, *offset) + ZBEE_ZCL_NSTIME_UTC_OFFSET;
    impl_time.nsecs = 0;
    proto_tree_add_time(tree, hf_zbee_zcl_msg_earliest_time, tvb, *offset, 4, &impl_time);
    *offset += 4;

} /
