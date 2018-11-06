static void
dissect_fcels_logi (tvbuff_t *tvb, packet_info *pinfo _U_, proto_tree *tree,
                    proto_item *ti, guint8 opcode)
{
    /
    int offset = 0,
        svcvld = 0,
        class;
    proto_tree *logi_tree, *cmnsvc_tree;
    proto_item *subti;
    gchar flagstr[256];
    guint16 flag;
    
    if (tree) {
        logi_tree = proto_item_add_subtree (ti, ett_fcels_logi);
        proto_tree_add_item (logi_tree, hf_fcels_opcode, tvb, offset, 1, FALSE);
        
        subti = proto_tree_add_text (logi_tree, tvb, offset+4, 16,
                                     "Common Svc Parameters");
        cmnsvc_tree = proto_item_add_subtree (subti, ett_fcels_logi_cmnsvc);
        proto_tree_add_item (cmnsvc_tree, hf_fcels_b2b, tvb, offset+6, 2, FALSE);
        flag = tvb_get_ntohs (tvb, offset+8);
        
        if (flag & 0x0001) {
            svcvld = 1;
        }

        construct_cmnsvc_string (flag, flagstr, opcode);
        proto_tree_add_uint_format (cmnsvc_tree, hf_fcels_cmnfeatures, tvb,
                                    offset+8, 2, flag,
                                    "Common Svc Parameters: 0x%x (%s)",
                                    flag, flagstr);
        
        proto_tree_add_item (cmnsvc_tree, hf_fcels_bbscnum, tvb, offset+10, 1, FALSE);
        proto_tree_add_item (cmnsvc_tree, hf_fcels_rcvsize, tvb, offset+10, 2, FALSE);
        proto_tree_add_item (cmnsvc_tree, hf_fcels_maxconseq, tvb, offset+12, 2, FALSE);
        proto_tree_add_item (cmnsvc_tree, hf_fcels_reloffset, tvb, offset+14, 2, FALSE);
        proto_tree_add_item (cmnsvc_tree, hf_fcels_edtov, tvb, offset+16, 4, FALSE);
        proto_tree_add_string (cmnsvc_tree, hf_fcels_npname, tvb, offset+20, 8,
                               fcwwn_to_str (tvb_get_ptr (tvb, offset+20, 8)));
        proto_tree_add_string (cmnsvc_tree, hf_fcels_fnname, tvb, offset+28, 8,
                               fcwwn_to_str (tvb_get_ptr (tvb, offset+28, 8)));

        /
        offset = 36;
        for (class = 1; class < 5; class++) {
            subti = proto_tree_add_text (logi_tree, tvb, offset, 16,
                                         "Class %d Svc Parameters", class);
            cmnsvc_tree = proto_item_add_subtree (subti, ett_fcels_logi_cmnsvc);

            flag = tvb_get_ntohs (tvb, offset);
            construct_clssvc_string (flag, flagstr, opcode);
            proto_tree_add_uint_format (cmnsvc_tree, hf_fcels_clsflags, tvb,
                                        offset, 2, flag,
                                        "Service Options: 0x%x(%s)", flag,
                                        flagstr);
            if (flag & 0x8000) {
                flag = tvb_get_ntohs (tvb, offset+2);
                construct_initctl_string (flag, flagstr, opcode);
                proto_tree_add_uint_format (cmnsvc_tree, hf_fcels_initctl, tvb,
                                            offset+2, 2, flag,
                                            "Initiator Control: 0x%x(%s)", flag,
                                            flagstr);

                flag = tvb_get_ntohs (tvb, offset+4);
                construct_rcptctl_string (flag, flagstr, opcode);
                proto_tree_add_uint_format (cmnsvc_tree, hf_fcels_initctl, tvb,
                                            offset+4, 2, flag,
                                            "Recipient Control: 0x%x(%s)", flag,
                                            flagstr);

                proto_tree_add_item (cmnsvc_tree, hf_fcels_clsrcvsize, tvb,
                                     offset+6, 2, FALSE);
                proto_tree_add_item (cmnsvc_tree, hf_fcels_conseq, tvb,
                                     offset+8, 2, FALSE);
                proto_tree_add_item (cmnsvc_tree, hf_fcels_e2e, tvb,
                                     offset+10, 2, FALSE);
                proto_tree_add_item (cmnsvc_tree, hf_fcels_openseq, tvb,
                                     offset+12, 2, FALSE);
            }
            offset += 16;
        }
        proto_tree_add_item (logi_tree, hf_fcels_vendorvers, tvb, offset, 16, FALSE);
        if (svcvld) {
            proto_tree_add_item (logi_tree, hf_fcels_svcavail, tvb, offset+32, 8, FALSE);
        }
    }
}
