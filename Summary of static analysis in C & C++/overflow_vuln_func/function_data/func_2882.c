static void
iscsi_connect_cb(struct iscsi_context *iscsi, int status, void *command_data,
                 void *opaque)
{
    struct IscsiTask *itask = opaque;
    struct scsi_task *task;

    if (status != 0) {
        itask->status   = 1;
        itask->complete = 1;
        return;
    }

    task = iscsi_readcapacity10_task(iscsi, itask->iscsilun->lun, 0, 0,
                                   iscsi_readcapacity10_cb, opaque);
    if (task == NULL) {
        error_report("iSCSI: failed to send readcapacity command.");
        itask->status   = 1;
        itask->complete = 1;
        return;
    }
}
