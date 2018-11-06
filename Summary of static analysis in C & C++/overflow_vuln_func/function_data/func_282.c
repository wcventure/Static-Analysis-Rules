static void
cops_analyze_packetcable_mm_obj(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint8 op_code, guint32 offset) {

    gint remdata;
    guint16 object_len;
    guint8 s_num, s_type;
    guint16 num_type_glob;

    /
    if ( cops_packetcable == FALSE ) {
       return;
    }

    /
    remdata = tvb_length_remaining(tvb, offset);
    while (remdata > 4) {

       /
       object_len   = tvb_get_ntohs(tvb, offset);
       if (object_len < 4) {
	proto_tree_add_text(tree, tvb, offset, 2,
	    "Incorrect PacketCable object length %u < 4", object_len);
	return;
       }

       s_num        = tvb_get_guint8(tvb, offset + 2);
       s_type       = tvb_get_guint8(tvb, offset + 3);

       /
       num_type_glob = s_num << 8 | s_type;

       /
       switch (num_type_glob){
        case PCMM_TRANSACTION_ID:
               cops_mm_transaction_id(tvb, pinfo, tree, op_code, object_len, offset);
               break;
        case PCMM_AMID:
               cops_amid(tvb, tree, object_len, offset);
               break;
        case PCMM_SUBSCRIBER_ID:
               cops_subscriber_id_v4(tvb, tree, object_len, offset);
               break;
        case PCMM_GATE_ID:
               cops_gate_id(tvb, tree, object_len, offset);
               break;
        case PCMM_GATE_SPEC:
               cops_mm_gate_spec(tvb, tree, object_len, offset);
               break;
        case PCMM_CLASSIFIER:
               cops_classifier(tvb, tree, object_len, offset);
               break;
        case PCMM_FLOW_SPEC:
               cops_flow_spec(tvb, tree, object_len, offset);
               break;
        case PCMM_DOCSIS_SERVICE_CLASS_NAME:
               cops_docsis_service_class_name(tvb, tree, object_len, offset);
               break;
        case PCMM_BEST_EFFORT_SERVICE:
               cops_best_effort_service(tvb, tree, object_len, offset);
               break;
        case PCMM_NON_REAL_TIME_POLLING_SERVICE:
               cops_non_real_time_polling_service(tvb, tree, object_len, offset);
               break;
        case PCMM_REAL_TIME_POLLING_SERVICE:
               cops_real_time_polling_service(tvb, tree, object_len, offset);
               break;
        case PCMM_UNSOLICITED_GRANT_SERVICE:
               cops_unsolicited_grant_service(tvb, tree, object_len, offset);
               break;
        case PCMM_UGS_WITH_ACTIVITY_DETECTION:
               cops_ugs_with_activity_detection(tvb, tree, object_len, offset);
               break;
        case PCMM_DOWNSTREAM_SERVICE:
               cops_downstream_service(tvb, tree, object_len, offset);
               break;
        case PCMM_EVENT_GENERATION_INFO:
               cops_mm_event_generation_info(tvb, tree, object_len, offset);
               break;
        case PCMM_VOLUME_BASED_USAGE_LIMIT:
               cops_volume_based_usage_limit(tvb, tree, object_len, offset);
               break;
        case PCMM_TIME_BASED_USAGE_LIMIT:
               cops_time_based_usage_limit(tvb, tree, object_len, offset);
               break;
        case PCMM_OPAQUE_DATA:
               cops_opaque_data(tvb, tree, object_len, offset);
               break;
        case PCMM_GATE_TIME_INFO:
               cops_gate_time_info(tvb, tree, object_len, offset);
               break;
        case PCMM_GATE_USAGE_INFO:
               cops_gate_usage_info(tvb, tree, object_len, offset);
               break;
        case PCMM_PACKETCABLE_ERROR:
               cops_packetcable_mm_error(tvb, tree, object_len, offset);
               break;
        case PCMM_GATE_STATE:
               cops_gate_state(tvb, tree, object_len, offset);
               break;
        case PCMM_VERSION_INFO:
               cops_version_info(tvb, tree, object_len, offset);
               break;
       }

       /
       offset += object_len;

       /
       remdata = tvb_length_remaining(tvb, offset);
    }
}
