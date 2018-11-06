static void dissect_wlan_if_stats(guint32 starting_offset, proto_item *parent_tree, tvbuff_t *tvb)
{
    proto_item *phy_types;
    proto_tree *phy_types_tree;
    guint32 phy_types_bitfield, noise_floor;

    proto_tree_add_item(parent_tree,
        hf_waveagent_ifwlanbssid, tvb, starting_offset, 6, ENC_NA);

    /

    proto_tree_add_item(parent_tree,
        hf_waveagent_ifwlanssid, tvb, starting_offset + 8, 32, ENC_ASCII|ENC_NA);

    /

    proto_tree_add_item(parent_tree,
        hf_waveagent_ifwlanrssi, tvb, starting_offset + 44, 4, ENC_BIG_ENDIAN);

    noise_floor = tvb_get_ntohl(tvb, starting_offset + 48);

    if (noise_floor != 0x7fffffff) {
        proto_tree_add_item(parent_tree,
            hf_waveagent_ifwlannoise, tvb, starting_offset + 48, 4, ENC_BIG_ENDIAN);
    }
    else {
        proto_tree_add_int_format(parent_tree, 
            hf_waveagent_ifwlannoise, tvb, starting_offset + 48, 4, noise_floor,
            "WLAN Interface Noise Floor (dBm): Not Reported");
    }

    phy_types_bitfield = tvb_get_ntohl(tvb, starting_offset + 52);

    phy_types = proto_tree_add_uint(parent_tree, hf_waveagent_ifphytypes,
                                tvb, starting_offset + 52, 4, phy_types_bitfield);

    phy_types_tree = proto_item_add_subtree(phy_types, ett_phytypes);

    proto_tree_add_item(phy_types_tree,
            hf_waveagent_ifphytypebit0, tvb, starting_offset + 55, 1, ENC_LITTLE_ENDIAN);

    proto_tree_add_item(phy_types_tree,
            hf_waveagent_ifphytypebit1, tvb, starting_offset + 55, 1, ENC_LITTLE_ENDIAN);

    proto_tree_add_item(phy_types_tree,
            hf_waveagent_ifphytypebit2, tvb, starting_offset + 55, 1, ENC_LITTLE_ENDIAN);

    proto_tree_add_item(phy_types_tree,
            hf_waveagent_ifphytypebit3, tvb, starting_offset + 55, 1, ENC_LITTLE_ENDIAN);

    proto_tree_add_item(parent_tree,
        hf_waveagent_ifwlanauthentication, tvb, starting_offset + 56, 4, ENC_BIG_ENDIAN);

    proto_tree_add_item(parent_tree,
        hf_waveagent_ifwlancipher, tvb, starting_offset + 60, 4, ENC_BIG_ENDIAN);
}
