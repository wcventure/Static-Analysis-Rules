static void
dissect_fcfcs_gieil (tvbuff_t *tvb, proto_tree *tree, gboolean isreq)
{
    int offset = 16; /
    int len;
    guint32 tot_len, prevlen;

    if (tree) {
        if (isreq) {
            proto_tree_add_item (tree, hf_fcs_iename, tvb, offset, 8, ENC_NA);
        }
        else {
            proto_tree_add_item_ret_uint(tree, hf_fcfcs_list_length, tvb, offset+3, 1, ENC_NA, &tot_len);

            prevlen = 0;
            len = tvb_strsize(tvb, offset+4);
            proto_tree_add_item (tree, hf_fcs_vendorname, tvb, offset+4,
                                 len, ENC_ASCII|ENC_NA);
            prevlen += len;

            len = tvb_strsize(tvb, offset+4+prevlen);
            proto_tree_add_item (tree, hf_fcs_modelname, tvb, offset+4+prevlen,
                                 len, ENC_ASCII|ENC_NA);
            prevlen += len;

            len = tvb_strsize(tvb, offset+4+prevlen);
            proto_tree_add_item (tree, hf_fcs_releasecode, tvb,
                                 offset+4+prevlen, len, ENC_ASCII|ENC_NA);
            prevlen += len;
            offset += (4+prevlen);
            while (tot_len > prevlen) {
                len = tvb_strsize(tvb, offset);
                proto_tree_add_item(tree, hf_fcfcs_vendor_specific_information, tvb, offset, len, ENC_NA|ENC_ASCII);
                prevlen += len;
                offset += len;
            }
        }
    }
}
