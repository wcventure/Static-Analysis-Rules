static void reset_q931_packet_info(q931_packet_info *pi)
{
    if(pi == NULL) {
        return;
    }

    g_free(pi->calling_number);
    g_free(pi->called_number);
    pi->calling_number = NULL;
    pi->called_number = NULL;
    pi->cause_value = 0xFF;
    pi->crv = -1;
}
