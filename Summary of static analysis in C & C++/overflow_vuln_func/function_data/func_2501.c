static void iscsi_timed_set_events(void *opaque)
{
    IscsiLun *iscsilun = opaque;
    iscsi_set_events(iscsilun);
}
