static int
res_add_input_data(tvbuff_t* tvb, proto_tree *ndps_tree, int foffset)
{
    guint32     resource_type=0;

    resource_type = tvb_get_ntohl(tvb, foffset);
    proto_tree_add_uint(ndps_tree, hf_res_type, tvb, foffset, 4, resource_type);
    foffset += 4;
    switch (resource_type) 
    {
    case 0:     /
        proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, FALSE);
        foffset += 4;
        foffset = ndps_string(tvb, hf_ndps_prn_dir_name, ndps_tree, foffset, NULL, 0);
        foffset = ndps_string(tvb, hf_ndps_prn_file_name, ndps_tree, foffset, NULL, 0);
        break;
    case 1:     /
        foffset = ndps_string(tvb, hf_ndps_vendor_dir, ndps_tree, foffset, NULL, 0);
        foffset = ndps_string(tvb, hf_ndps_prn_file_name, ndps_tree, foffset, NULL, 0);
        break;
    case 2:     /
        foffset = ndps_string(tvb, hf_ndps_banner_name, ndps_tree, foffset, NULL, 0);
        break;
    case 3:     /
        proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, FALSE);
        foffset += 4;
        proto_tree_add_item(ndps_tree, hf_font_type, tvb, foffset, 4, FALSE);
        foffset += 4;
        foffset = ndps_string(tvb, hf_ndps_prn_file_name, ndps_tree, foffset, NULL, 0);
        break;
    case 4:     /
    case 5:     /
        proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, FALSE);
        foffset += 4;
        foffset = ndps_string(tvb, hf_ndps_prn_dir_name, ndps_tree, foffset, NULL, 0);
        proto_tree_add_item(ndps_tree, hf_archive_type, tvb, foffset, 4, FALSE);
        foffset += 4;
        break;
    default:
        break;
    }
    return foffset;
}
