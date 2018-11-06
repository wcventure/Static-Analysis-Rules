static void ipmi_init_sensors_from_sdrs(IPMIBmcSim *s)
{
    unsigned int i, pos;
    IPMISensor *sens;

    for (i = 0; i < MAX_SENSORS; i++) {
        memset(s->sensors + i, 0, sizeof(*sens));
    }

    pos = 0;
    for (i = 0; !sdr_find_entry(&s->sdr, i, &pos, NULL); i++) {
        struct ipmi_sdr_compact *sdr =
            (struct ipmi_sdr_compact *) &s->sdr.sdr[pos];
        unsigned int len = sdr->header.rec_length;

        if (len < 20) {
            continue;
        }
        if (sdr->header.rec_type != IPMI_SDR_COMPACT_TYPE) {
            continue; /
        }

        if (sdr->sensor_owner_number > MAX_SENSORS) {
            continue;
        }
        sens = s->sensors + sdr->sensor_owner_number;

        IPMI_SENSOR_SET_PRESENT(sens, 1);
        IPMI_SENSOR_SET_SCAN_ON(sens, (sdr->sensor_init >> 6) & 1);
        IPMI_SENSOR_SET_EVENTS_ON(sens, (sdr->sensor_init >> 5) & 1);
        sens->assert_suppt = sdr->assert_mask[0] | (sdr->assert_mask[1] << 8);
        sens->deassert_suppt =
            sdr->deassert_mask[0] | (sdr->deassert_mask[1] << 8);
        sens->states_suppt =
            sdr->discrete_mask[0] | (sdr->discrete_mask[1] << 8);
        sens->sensor_type = sdr->sensor_type;
        sens->evt_reading_type_code = sdr->reading_type & 0x7f;

        /
        sens->assert_enable = sens->assert_suppt;
        sens->deassert_enable = sens->deassert_suppt;
    }
}
