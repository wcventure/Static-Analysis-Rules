static int of_dpa_cmd_add_l2_flood(OfDpa *of_dpa, OfDpaGroup *group,
                                   RockerTlv **group_tlvs)
{
    OfDpaGroup *l2_group;
    RockerTlv **tlvs;
    int err;
    int i;

    if (!group_tlvs[ROCKER_TLV_OF_DPA_GROUP_COUNT] ||
        !group_tlvs[ROCKER_TLV_OF_DPA_GROUP_IDS]) {
        return -ROCKER_EINVAL;
    }

    group->l2_flood.group_count =
        rocker_tlv_get_le16(group_tlvs[ROCKER_TLV_OF_DPA_GROUP_COUNT]);

    tlvs = g_malloc0((group->l2_flood.group_count + 1) *
                     sizeof(RockerTlv *));
    if (!tlvs) {
        return -ROCKER_ENOMEM;
    }

    g_free(group->l2_flood.group_ids);
    group->l2_flood.group_ids =
        g_malloc0(group->l2_flood.group_count * sizeof(uint32_t));
    if (!group->l2_flood.group_ids) {
        err = -ROCKER_ENOMEM;
        goto err_out;
    }

    rocker_tlv_parse_nested(tlvs, group->l2_flood.group_count,
                            group_tlvs[ROCKER_TLV_OF_DPA_GROUP_IDS]);

    for (i = 0; i < group->l2_flood.group_count; i++) {
        group->l2_flood.group_ids[i] = rocker_tlv_get_le32(tlvs[i + 1]);
    }

    /

    for (i = 0; i < group->l2_flood.group_count; i++) {
        l2_group = of_dpa_group_find(of_dpa, group->l2_flood.group_ids[i]);
        if (!l2_group) {
            continue;
        }
        if ((ROCKER_GROUP_TYPE_GET(l2_group->id) ==
             ROCKER_OF_DPA_GROUP_TYPE_L2_INTERFACE) &&
            (ROCKER_GROUP_VLAN_GET(l2_group->id) !=
             ROCKER_GROUP_VLAN_GET(group->id))) {
            DPRINTF("l2 interface group 0x%08x VLAN doesn't match l2 "
                    "flood group 0x%08x\n",
                    group->l2_flood.group_ids[i], group->id);
            err = -ROCKER_EINVAL;
            goto err_out;
        }
    }

    g_free(tlvs);
    return ROCKER_OK;

err_out:
    group->l2_flood.group_count = 0;
    g_free(group->l2_flood.group_ids);
    g_free(tlvs);

    return err;
}
