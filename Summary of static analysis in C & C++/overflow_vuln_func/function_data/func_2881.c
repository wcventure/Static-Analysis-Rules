static void
iscsi_readcapacity10_cb(struct iscsi_context *iscsi, int status,
                        void *command_data, void *opaque)
{
    struct IscsiTask *itask = opaque;
    struct scsi_readcapacity10 *rc10;
    struct scsi_task *task = command_data;

    if (status != 0) {
        error_report("iSCSI: Failed to read capacity of iSCSI lun. %s",
                     iscsi_get_error(iscsi));
        itask->status   = 1;
        itask->complete = 1;
        scsi_free_scsi_task(task);
        return;
    }

    rc10 = scsi_datain_unmarshall(task);
    if (rc10 == NULL) {
        error_report("iSCSI: Failed to unmarshall readcapacity10 data.");
        itask->status   = 1;
        itask->complete = 1;
        scsi_free_scsi_task(task);
        return;
    }

    itask->iscsilun->block_size = rc10->block_size;
    itask->iscsilun->num_blocks = rc10->lba;
    itask->bs->total_sectors = (uint64_t)rc10->lba *
                               rc10->block_size / BDRV_SECTOR_SIZE ;

    itask->status   = 0;
    itask->complete = 1;
    scsi_free_scsi_task(task);
}
