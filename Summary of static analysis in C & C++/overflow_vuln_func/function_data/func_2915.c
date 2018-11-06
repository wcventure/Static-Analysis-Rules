static void set_sensor_evt_enable(IPMIBmcSim *ibs,
                                  uint8_t *cmd, unsigned int cmd_len,
                                  uint8_t *rsp, unsigned int *rsp_len,
                                  unsigned int max_rsp_len)
{
    IPMISensor *sens;

    IPMI_CHECK_CMD_LEN(4);
    if ((cmd[2] > MAX_SENSORS) ||
            !IPMI_SENSOR_GET_PRESENT(ibs->sensors + cmd[2])) {
        rsp[2] = IPMI_CC_REQ_ENTRY_NOT_PRESENT;
        return;
    }
    sens = ibs->sensors + cmd[2];
    switch ((cmd[3] >> 4) & 0x3) {
    case 0: /
        break;
    case 1: /
        if (cmd_len > 4) {
            sens->assert_enable |= cmd[4];
        }
        if (cmd_len > 5) {
            sens->assert_enable |= cmd[5] << 8;
        }
        if (cmd_len > 6) {
            sens->deassert_enable |= cmd[6];
        }
        if (cmd_len > 7) {
            sens->deassert_enable |= cmd[7] << 8;
        }
        break;
    case 2: /
        if (cmd_len > 4) {
            sens->assert_enable &= ~cmd[4];
        }
        if (cmd_len > 5) {
            sens->assert_enable &= ~(cmd[5] << 8);
        }
        if (cmd_len > 6) {
            sens->deassert_enable &= ~cmd[6];
        }
        if (cmd_len > 7) {
            sens->deassert_enable &= ~(cmd[7] << 8);
        }
        break;
    case 3:
        rsp[2] = IPMI_CC_INVALID_DATA_FIELD;
        return;
    }
    IPMI_SENSOR_SET_RET_STATUS(sens, cmd[3]);
}
