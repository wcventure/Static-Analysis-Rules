gint dissect_ulmap_ie( proto_tree *ie_tree, const guint8 *bufptr, gint offset, gint length _U_, tvbuff_t *tvb)
{
    /
    proto_item *ti = NULL;
    proto_tree *tree = NULL;
    gint nibble;
    gint uiuc, ext_uiuc, ext2_uiuc, len, aas_or_amc;
    guint cid;
    guint data;
    guint32 data32;

    nibble = offset;

    /
    cid = NIB_WORD(nibble, bufptr);
    uiuc = NIB_NIBBLE(nibble + 4, bufptr);

    if (uiuc == 0)
    {
        /
        ti = proto_tree_add_text(ie_tree, tvb, NIBHI(nibble, 5+8), "FAST FEEDBACK Allocation IE");
        tree = proto_item_add_subtree(ti, ett_ulmap_ffb);

        proto_tree_add_uint(tree, hf_ulmap_ie_cid, tvb, NIBHI(nibble, 4), cid);
        nibble += 4;
        proto_tree_add_uint(tree, hf_ulmap_ie_uiuc, tvb, NIBHI(nibble, 1), uiuc);
        nibble += 1;

        data = NIB_LONG(nibble, bufptr);
        proto_tree_add_uint(tree, hf_ulmap_uiuc0_symofs, tvb, NIBHI(nibble, 8), data);
        proto_tree_add_uint(tree, hf_ulmap_uiuc0_subofs, tvb, NIBHI(nibble, 8), data);
        proto_tree_add_uint(tree, hf_ulmap_uiuc0_numsym, tvb, NIBHI(nibble, 8), data);
        proto_tree_add_uint(tree, hf_ulmap_uiuc0_numsub, tvb, NIBHI(nibble, 8), data);
        proto_tree_add_uint(tree, hf_ulmap_uiuc0_rsv,    tvb, NIBHI(nibble, 8), data);
        nibble += 8;
    }
    else if (uiuc == 11)
    {
        /
        ext2_uiuc = NIB_NIBBLE(5+nibble, bufptr);
        len = NIB_BYTE(5+nibble+1, bufptr);

        ti = proto_tree_add_text(ie_tree, tvb, NIBHI(nibble, 5+3+len*2), "UIUC: %d (Extended-2 IE)", uiuc);
        tree = proto_item_add_subtree(ti, ett_290b);

        proto_tree_add_uint(tree, hf_ulmap_ie_cid, tvb, NIBHI(nibble, 4), cid);
        nibble += 4;
        proto_tree_add_uint(tree, hf_ulmap_ie_uiuc, tvb, NIBHI(nibble, 1), uiuc);
        nibble += 1;

#if 0
        proto_tree_add_uint(tree, hf_ulmap_uiuc11_ext, tvb, NIBHI(nibble, 1), ext2_uiuc);
        nibble += 1;
        proto_tree_add_uint(tree, hf_ulmap_uiuc11_len, tvb, NIBHI(nibble, 2), len);
        nibble += 2;
#endif

        len = 3 + BYTE_TO_NIB(len); /

        /
        switch (ext2_uiuc) {
            case 0x00:
                /
                nibble = CQICH_Enhanced_Allocation_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x01:
                /
                nibble = HO_Anchor_Active_UL_MAP_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x02:
                /
                nibble = HO_Active_Anchor_UL_MAP_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x03:
                /
                nibble = Anchor_BS_switch_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x04:
                /
                nibble = UL_sounding_command_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x06:
                /
                nibble = MIMO_UL_Enhanced_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x07:
                /
                nibble = HARQ_ULMAP_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x08:
                /
                nibble = HARQ_ACKCH_Region_Allocation_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x0e:
                /
                nibble = AAS_SDMA_UL_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x0f:
                /
                nibble = Feedback_Polling_IE(tree, bufptr, nibble, len, tvb);
                break;

            default:
                proto_tree_add_text(tree, tvb, NIBHI(nibble, len), "(reserved Extended-2 UIUC: %d)", ext2_uiuc);
		nibble += len;
                break;

        }
    }
    else if (uiuc == 12)
    {
        /
        ti = proto_tree_add_text(ie_tree, tvb, NIBHI(nibble, 5+8), "CDMA Bandwidth/Ranging Request IE");
        tree = proto_item_add_subtree(ti, ett_287_1);

        proto_tree_add_uint(tree, hf_ulmap_ie_cid, tvb, NIBHI(nibble, 4), cid);
        nibble += 4;
        proto_tree_add_uint(tree, hf_ulmap_ie_uiuc, tvb, NIBHI(nibble, 1), uiuc);
        nibble += 1;

        data32 = NIB_LONG(nibble, bufptr);
        proto_tree_add_uint(tree, hf_ulmap_uiuc12_symofs, tvb, NIBHI(nibble,8), data32);
        proto_tree_add_uint(tree, hf_ulmap_uiuc12_subofs, tvb, NIBHI(nibble,8), data32);
        proto_tree_add_uint(tree, hf_ulmap_uiuc12_numsym, tvb, NIBHI(nibble,8), data32);
        proto_tree_add_uint(tree, hf_ulmap_uiuc12_numsub, tvb, NIBHI(nibble,8), data32);
        proto_tree_add_uint(tree, hf_ulmap_uiuc12_method, tvb, NIBHI(nibble,8), data32);
        proto_tree_add_uint(tree, hf_ulmap_uiuc12_dri,    tvb, NIBHI(nibble,8), data32);
        nibble += 8;
    }
    else if (uiuc == 13)
    {
        /
        ti = proto_tree_add_text(ie_tree, tvb, NIBHI(nibble,5+8), "PAPR/Safety/Sounding Zone IE");
        tree = proto_item_add_subtree(ti, ett_289);


        proto_tree_add_uint(tree, hf_ulmap_ie_cid, tvb, NIBHI(nibble, 4), cid);
        nibble += 4;
        proto_tree_add_uint(tree, hf_ulmap_ie_uiuc, tvb, NIBHI(nibble, 1), uiuc);
        nibble += 1;

        data = NIB_LONG(nibble, bufptr);
        proto_tree_add_uint(tree, hf_ulmap_uiuc13_symofs, tvb, NIBHI(nibble,8), data);
        proto_tree_add_uint(tree, hf_ulmap_uiuc13_subofs, tvb, NIBHI(nibble,8), data);
        proto_tree_add_uint(tree, hf_ulmap_uiuc13_numsym, tvb, NIBHI(nibble,8), data);
        proto_tree_add_uint(tree, hf_ulmap_uiuc13_numsub, tvb, NIBHI(nibble,8), data);
        proto_tree_add_uint(tree, hf_ulmap_uiuc13_papr,   tvb, NIBHI(nibble,8), data);
        proto_tree_add_uint(tree, hf_ulmap_uiuc13_zone,   tvb, NIBHI(nibble,8), data);
        proto_tree_add_uint(tree, hf_ulmap_uiuc13_rsv,    tvb, NIBHI(nibble,8), data);
        nibble += 8;
    }
    else if (uiuc == 14)
    {
        /
        ti = proto_tree_add_text(ie_tree, tvb, NIBHI(nibble,5+10), "CDMA allocation IE");
        tree = proto_item_add_subtree(ti, ett_290);

        proto_tree_add_uint(tree, hf_ulmap_ie_cid, tvb, NIBHI(nibble, 4), cid);
        nibble += 4;
        proto_tree_add_uint(tree, hf_ulmap_ie_uiuc, tvb, NIBHI(nibble, 1), uiuc);
        nibble += 1;

        data = NIB_WORD(nibble, bufptr);
        proto_tree_add_uint(tree, hf_ulmap_uiuc14_dur,  tvb, NIBHI(nibble,2), data);
        proto_tree_add_uint(tree, hf_ulmap_uiuc14_uiuc, tvb, NIBHI(nibble+1,2), data);
        proto_tree_add_uint(tree, hf_ulmap_uiuc14_rep,  tvb, NIBHI(nibble+2,1), data);
        proto_tree_add_uint(tree, hf_ulmap_uiuc14_idx,  tvb, NIBHI(nibble+3,1), data);
        nibble += 4;

        data = NIB_BYTE(nibble, bufptr);
        proto_tree_add_uint(tree, hf_ulmap_uiuc14_code, tvb, NIBHI(nibble,2), data);
        proto_item_append_text(ti, " (0x%02x)", data);
        nibble += 2;

        data = NIB_BYTE(nibble, bufptr);
        proto_tree_add_uint(tree, hf_ulmap_uiuc14_sym,  tvb, NIBHI(nibble,2), data);
        proto_item_append_text(ti, " (0x%02x)", data);
        nibble += 2;

        data = NIB_BYTE(nibble, bufptr);
        proto_tree_add_uint(tree, hf_ulmap_uiuc14_sub,  tvb, NIBHI(nibble,2), data);
        proto_item_append_text(ti, " (0x%02x)", data >> 1);
        proto_tree_add_uint(tree, hf_ulmap_uiuc14_bwr,  tvb, NIBHI(nibble+1,1), data);
        nibble += 2;
    }
    else if (uiuc == 15)
    {
        /
        ext_uiuc = NIB_NIBBLE(5+nibble, bufptr);
        len = NIB_NIBBLE(5+nibble+1, bufptr);

        ti = proto_tree_add_text(ie_tree, tvb, NIBHI(nibble, 5+2+len*2), "UIUC: %d (Extended IE)", uiuc);
        tree = proto_item_add_subtree(ti, ett_291);

        proto_tree_add_uint(tree, hf_ulmap_ie_cid, tvb, NIBHI(nibble,4), cid);
        nibble += 4;
        proto_tree_add_uint(tree, hf_ulmap_ie_uiuc, tvb, NIBHI(nibble,1), uiuc);
        nibble += 1;

#if 0
        ti = proto_tree_add_uint(tree, hf_ulmap_uiuc11_ext, tvb, NIBHI(nibble,1), ext_uiuc);
        nibble += 1;
        proto_tree_add_uint(tree, hf_ulmap_uiuc11_len, tvb, NIBHI(nibble,1), len);
        nibble += 1;
#endif

        len = 2 + BYTE_TO_NIB(len); /

        /
        switch (ext_uiuc) {
            case 0x00:
                /
                nibble = Power_Control_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x01:
                /
                nibble = Mini_Subchannel_allocation_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x02:
                /
                nibble = AAS_UL_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x03:
                /
                nibble = CQICH_Alloc_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x04:
                /
                nibble = UL_Zone_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x05:
                /
                nibble = PHYMOD_UL_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x06:
                /
                nibble = MIMO_UL_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x07:
                /
                nibble = ULMAP_Fast_Tracking_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x08:
                /
                nibble = UL_PUSC_Burst_Allocation_in_other_segment_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x09:
                /
                nibble = Fast_Ranging_IE(tree, bufptr, nibble, len, tvb);
                break;
            case 0x0a:
                /
                nibble = UL_Allocation_Start_IE(tree, bufptr, nibble, len, tvb);
                break;
            default:
                proto_tree_add_text(tree, tvb, NIBHI(nibble,len), "(reserved Extended UIUC: %d)", ext_uiuc);
		nibble += len;
                break;
        }
    }
    else
    {
        /
        aas_or_amc = 0; /
        len = 3;

        if (aas_or_amc) len += 3;

        ti = proto_tree_add_text(ie_tree, tvb, NIBHI(nibble, 5+len), "Data Grant Burst Profile");
        tree = proto_item_add_subtree(ti, ett_287_2);

        proto_tree_add_uint(tree, hf_ulmap_ie_cid, tvb, NIBHI(nibble, 4), cid);
        nibble += 4;
        proto_tree_add_uint(tree, hf_ulmap_ie_uiuc, tvb, NIBHI(nibble, 1), uiuc);
        nibble += 1;

        data = NIB_WORD(nibble, bufptr);
        proto_tree_add_uint(tree, hf_ulmap_uiuc10_dur, tvb, NIBHI(nibble,3), data);
        proto_tree_add_uint(tree, hf_ulmap_uiuc10_rep, tvb, NIBHI(nibble+2,1), data);
        nibble += 3;

        if (aas_or_amc) {
            data = NIB_BITS12(nibble, bufptr);
            proto_tree_add_text(tree, tvb, NIBHI(nibble,3), "Slot offset: %d", data);
            nibble += 3;
        }
    }

    /
    return (nibble - offset);
}
