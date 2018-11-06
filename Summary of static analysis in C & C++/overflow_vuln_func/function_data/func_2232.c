SCSIRequest *scsi_req_new(SCSIDevice *d, uint32_t tag, uint32_t lun,
                          uint8_t *buf, void *hba_private)
{
    SCSIBus *bus = DO_UPCAST(SCSIBus, qbus, d->qdev.parent_bus);
    SCSIRequest *req;
    SCSICommand cmd;

    if (scsi_req_parse(&cmd, d, buf) != 0) {
        trace_scsi_req_parse_bad(d->id, lun, tag, buf[0]);
        req = scsi_req_alloc(&reqops_invalid_opcode, d, tag, lun, hba_private);
    } else {
        trace_scsi_req_parsed(d->id, lun, tag, buf[0],
                              cmd.mode, cmd.xfer);
        if (cmd.lba != -1) {
            trace_scsi_req_parsed_lba(d->id, lun, tag, buf[0],
                                      cmd.lba);
        }

        if ((d->unit_attention.key == UNIT_ATTENTION ||
             bus->unit_attention.key == UNIT_ATTENTION) &&
            (buf[0] != INQUIRY &&
             buf[0] != REPORT_LUNS &&
             buf[0] != GET_CONFIGURATION &&
             buf[0] != GET_EVENT_STATUS_NOTIFICATION &&

             /
             !(buf[0] == REQUEST_SENSE && d->sense_is_ua))) {
            req = scsi_req_alloc(&reqops_unit_attention, d, tag, lun,
                                 hba_private);
        } else if (lun != d->lun ||
            buf[0] == REPORT_LUNS ||
            (buf[0] == REQUEST_SENSE && (d->sense_len || cmd.xfer < 4))) {
            req = scsi_req_alloc(&reqops_target_command, d, tag, lun,
                                 hba_private);
        } else {
            req = scsi_device_alloc_req(d, tag, lun, buf, hba_private);
        }
    }

    req->cmd = cmd;
    req->resid = req->cmd.xfer;

    switch (buf[0]) {
    case INQUIRY:
        trace_scsi_inquiry(d->id, lun, tag, cmd.buf[1], cmd.buf[2]);
        break;
    case TEST_UNIT_READY:
        trace_scsi_test_unit_ready(d->id, lun, tag);
        break;
    case REPORT_LUNS:
        trace_scsi_report_luns(d->id, lun, tag);
        break;
    case REQUEST_SENSE:
        trace_scsi_request_sense(d->id, lun, tag);
        break;
    default:
        break;
    }

    return req;
}
