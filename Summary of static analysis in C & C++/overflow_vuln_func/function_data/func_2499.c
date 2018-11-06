static void
iscsi_co_generic_cb(struct iscsi_context *iscsi, int status,
                        void *command_data, void *opaque)
{
    struct IscsiTask *iTask = opaque;
    struct scsi_task *task = command_data;

    iTask->status = status;
    iTask->do_retry = 0;
    iTask->task = task;

    if (status != SCSI_STATUS_GOOD) {
        if (iTask->retries++ < ISCSI_CMD_RETRIES) {
            if (status == SCSI_STATUS_CHECK_CONDITION
                && task->sense.key == SCSI_SENSE_UNIT_ATTENTION) {
                error_report("iSCSI CheckCondition: %s",
                             iscsi_get_error(iscsi));
                iTask->do_retry = 1;
                goto out;
            }
            /
            if (status == SCSI_STATUS_BUSY || status == 0x28) {
                unsigned retry_time =
                    exp_random(iscsi_retry_times[iTask->retries - 1]);
                error_report("iSCSI Busy/TaskSetFull (retry #%u in %u ms): %s",
                             iTask->retries, retry_time,
                             iscsi_get_error(iscsi));
                aio_timer_init(iTask->iscsilun->aio_context,
                               &iTask->retry_timer, QEMU_CLOCK_REALTIME,
                               SCALE_MS, iscsi_retry_timer_expired, iTask);
                timer_mod(&iTask->retry_timer,
                          qemu_clock_get_ms(QEMU_CLOCK_REALTIME) + retry_time);
                iTask->do_retry = 1;
                return;
            }
        }
        error_report("iSCSI Failure: %s", iscsi_get_error(iscsi));
    } else {
        iTask->iscsilun->force_next_flush |= iTask->force_next_flush;
    }

out:
    if (iTask->co) {
        iTask->bh = aio_bh_new(iTask->iscsilun->aio_context,
                               iscsi_co_generic_bh_cb, iTask);
        qemu_bh_schedule(iTask->bh);
    } else {
        iTask->complete = 1;
    }
}
