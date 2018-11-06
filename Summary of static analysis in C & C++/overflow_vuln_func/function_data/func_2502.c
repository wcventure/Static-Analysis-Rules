static void iscsi_nop_timed_event(void *opaque)
{
    IscsiLun *iscsilun = opaque;

    if (iscsi_get_nops_in_flight(iscsilun->iscsi) > MAX_NOP_FAILURES) {
        error_report("iSCSI: NOP timeout. Reconnecting...");
        iscsi_reconnect(iscsilun->iscsi);
    }

    if (iscsi_nop_out_async(iscsilun->iscsi, NULL, NULL, 0, NULL) != 0) {
        error_report("iSCSI: failed to sent NOP-Out. Disabling NOP messages.");
        return;
    }

    timer_mod(iscsilun->nop_timer, qemu_clock_get_ms(QEMU_CLOCK_REALTIME) + NOP_INTERVAL);
    iscsi_set_events(iscsilun);
}
