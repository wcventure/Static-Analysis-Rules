static void get_sensor_reading(IPMIBmcSim *ibs,
                               uint8_t *cmd, unsigned int cmd_len,
                               uint8_t *rsp, unsigned int *rsp_len,
                               unsigned int max_rsp_len)
{
    IPMISensor *sens;

    IPMI_CHECK_CMD_LEN(3);
    if ((cmd[2] > MAX_SENSORS) ||
            !IPMI_SENSOR_GET_PRESENT(ibs->sensors + cmd[2])) {
        rsp[2] = IPMI_CC_REQ_ENTRY_NOT_PRESENT;
        return;
    }
    sens = ibs->sensors + cmd[2];
    IPMI_ADD_RSP_DATA(sens->reading);
    IPMI_ADD_RSP_DATA(IPMI_SENSOR_GET_RET_STATUS(sens));
    IPMI_ADD_RSP_DATA(sens->states & 0xff);
    if (IPMI_SENSOR_IS_DISCRETE(sens)) {
        IPMI_ADD_RSP_DATA((sens->states >> 8) & 0xff);
    }
}
