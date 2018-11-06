static int
return_code(tvbuff_t *tvb, packet_info *pinfo, proto_tree *ndps_tree, int foffset)
{
    expert_status = tvb_get_ntohl(tvb, foffset);
    expert_item = proto_tree_add_item(ndps_tree, hf_ndps_return_code, tvb, foffset, 4, FALSE);
    if (expert_status != 0) {
        expert_add_info_format(pinfo, expert_item, PI_RESPONSE_CODE, PI_ERROR, "Fault: %s", val_to_str(expert_status, ndps_error_types, "Unknown NDPS Error (0x%08x)"));
    }
    foffset += 4;
    if (check_col(pinfo->cinfo, COL_INFO) && tvb_get_ntohl(tvb, foffset-4) != 0)
        col_add_fstr(pinfo->cinfo, COL_INFO, "R NDPS - Error");
    if (tvb_get_ntohl(tvb, foffset-4) == 0) 
    {
        return foffset;
    }
    proto_tree_add_item(ndps_tree, hf_ndps_ext_error, tvb, foffset, 4, FALSE);
    foffset += 4;
    return foffset;
}
