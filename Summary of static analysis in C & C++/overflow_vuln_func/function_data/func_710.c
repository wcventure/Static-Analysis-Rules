static int
dissect_ieee802154_payload_mlme_sub_ie(tvbuff_t *tvb, packet_info *pinfo _U_, proto_tree *tree, int offset)
{
    guint16     psie_ie;
    guint16     psie_id;
    guint       psie_remaining = 0;
    int         orig_offset = offset;

    psie_ie    =  tvb_get_letohs(tvb, offset);
    if (psie_ie & IEEE802154_PSIE_TYPE_MASK) {
        /
        psie_id        = (guint16) ((psie_ie & IEEE802154_PSIE_ID_MASK_LONG) >> 11);
        psie_remaining = (guint) (psie_ie & IEEE802154_PSIE_LENGTH_MASK_LONG);
    }
    else {
        /
        psie_id        = (guint16) ((psie_ie & IEEE802154_PSIE_ID_MASK_SHORT) >> 8);
        psie_remaining = (guint) (psie_ie & IEEE802154_PSIE_LENGTH_MASK_SHORT);
    }

    switch (psie_id) {

        case IEEE802154_MLME_SUBIE_TSCH_SYNCH:
            // 7.4.4.2 TSCH Synchronization IE
            dissect_802154_tsch_time_sync(tvb, tree, &offset, psie_remaining);
            break;

        case IEEE802154_MLME_SUBIE_TSCH_SLOTFR_LINK:
            // 7.4.4.3 TSCH Slotframe and Link IE
            dissect_802154_tsch_slotframe_link(tvb, tree, psie_remaining, psie_id, &offset);
            break;

        case IEEE802154_MLME_SUBIE_TSCH_TIMESLOT:
            // 7.4.4.4 TSCH Timeslot IE
            dissect_802154_tsch_timeslot(tvb, tree, psie_remaining, psie_id, &offset);
            break;

        case IEEE802154_MLME_SUBIE_ENHANCED_BEACON_FILTER:
            // 7.4.4.6 Enhanced Beacon Filter IE
            dissect_802154_enhanced_beacon_filter(tvb, tree, psie_remaining, &offset);
            break;

        case IEEE802154_MLME_SUBIE_CHANNEL_HOPPING:
            // 7.4.4.31 Channel hopping IE
            dissect_802154_channel_hopping(tvb, tree, psie_remaining, &offset);
            break;

        case IEEE802154_MLME_SUBIE_HOPPING_TIMING:
            // 7.4.4.5 Hopping timing IE
            // TODO

        case IEEE802154_MLME_SUBIE_MAC_METRICS:
            // 7.4.4.7 MAC Metrics IE
            // TODO

        case IEEE802154_MLME_SUBIE_ALL_MAC_METRICS:
            // 7.4.4.8 All MAC Metrics IE
            // TODO

        case IEEE802154_MLME_SUBIE_COEXISTENCE_SPEC:
            // 7.4.4.9 Coexistence Specification IE
            // TODO

        case IEEE802154_MLME_SUBIE_SUN_DEVICE_CAPABILITIES:
            // 7.4.4.10 SUN Device Capabilities IE
            // TODO

        case IEEE802154_MLME_SUBIE_SUN_FSK_GEN_PHY:
            // 7.4.4.11 SUN FSK Generic PHY IE
            // TODO

        case IEEE802154_MLME_SUBIE_MODE_SWITCH_PARAMETER:
            // 7.4.4.12 Mode Switch Parameter IE
            // TODO

        case IEEE802154_MLME_SUBIE_PHY_PARAMETER_CHANGE:
            // 7.4.4.13 PHY Parameter Change IE
            // TODO

        case IEEE802154_MLME_SUBIE_O_QPSK_PHY_MODE:
            // 7.4.4.14 O-QPSK PHY Mode IE
            // TODO

        case IEEE802154_MLME_SUBIE_PCA_ALLOCATION:
            // 7.4.4.15 PCA Allocation IE
            // TODO

        case IEEE802154_MLME_SUBIE_DSSS_OPER_MODE:
            // 7.4.4.16 LECIM DSSS Operating Mode IE
            // TODO

        case IEEE802154_MLME_SUBIE_FSK_OPER_MODE:
            // 7.4.4.17 LECIM FSK Operating Mode IE
            // TODO

        case IEEE802154_MLME_SUBIE_TVWS_PHY_OPE_MODE:
            // 7.4.4.18 TVWS PHY Operating Mode Description IE
            // TODO

        case IEEE802154_MLME_SUBIE_TVWS_DEVICE_CAPAB:
            // 7.4.4.19 TVWS Device Capabilities IE
            // TODO

        case IEEE802154_MLME_SUBIE_TVWS_DEVICE_CATEG:
            // 7.4.4.20 TVWS Device Category IE
            // TODO

        case IEEE802154_MLME_SUBIE_TVWS_DEVICE_IDENTIF:
            // 7.4.4.21 TVWS Device Identification IE
            // TODO

        case IEEE802154_MLME_SUBIE_TVWS_DEVICE_LOCATION:
            // 7.4.4.22 TVWS Device Location IE
            // TODO

        case IEEE802154_MLME_SUBIE_TVWS_CH_INFOR_QUERY:
            // 7.4.4.23 TVWS Channel Information Query IE
            // TODO

        case IEEE802154_MLME_SUBIE_TVWS_CH_INFOR_SOURCE:
            // 7.4.4.24 TVWS Channel Information Source IE
            // TODO

        case IEEE802154_MLME_SUBIE_CTM:
            // 7.4.4.25 CTM IE
            // TODO

        case IEEE802154_MLME_SUBIE_TIMESTAMP:
            // 7.4.4.26 Timestamp IE
            // TODO

        case IEEE802154_MLME_SUBIE_TIMESTAMP_DIFF:
            // 7.4.4.27 Timestamp Difference IE
            // TODO

        case IEEE802154_MLME_SUBIE_TMCP_SPECIFICATION:
            // 7.4.4.28 TMCTP Specification IE
            // TODO

        case IEEE802154_MLME_SUBIE_RCC_PHY_OPER_MODE:
            // 7.4.4.29 RCC PHY Operating Mode IE
            // TODO

        default:
            offset += 2;
            if (psie_remaining) {
                proto_tree_add_item(tree, hf_ieee802154_mlme_ie_data, tvb, offset, psie_remaining, ENC_NA);
                expert_add_info(pinfo, tree, &ei_ieee802154_unsupported_element_id);
                offset += psie_remaining;
            }
            break;
    }

    return (offset - orig_offset);
}
