typedef struct _gsm_a_stat_t {
    int         bssmap_message_type[0xff];
    int         dtap_mm_message_type[0xff];
    int         dtap_rr_message_type[0xff];
    int         dtap_cc_message_type[0xff];
    int         dtap_gmm_message_type[0xff];
    int         dtap_sms_message_type[0xff];
    int         dtap_sm_message_type[0xff];
    int         dtap_ss_message_type[0xff];
    int         dtap_tp_message_type[0xff];
    int         sacch_rr_message_type[0xff];
} gsm_a_stat_t;
