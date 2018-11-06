static int
print_address(tvbuff_t* tvb, proto_tree *ndps_tree, int foffset)
{
    guint32     address_type=0;
    guint32     address_len=0;

    address_type = tvb_get_ntohl(tvb, foffset); 
    proto_tree_add_uint(ndps_tree, hf_ndps_address, tvb, foffset, 4, address_type);
    foffset += 4;
    address_len = tvb_get_ntohl(tvb, foffset);
    proto_tree_add_item(ndps_tree, hf_address_len, tvb, foffset, 4, FALSE);
    foffset += 4;
    /
    switch(address_type)
    {
    case 0x00000000:
        proto_tree_add_item(ndps_tree, hf_ndps_net, tvb, foffset, 4, FALSE);
        proto_tree_add_item(ndps_tree, hf_ndps_node, tvb, foffset+4, 6, FALSE);
        proto_tree_add_item(ndps_tree, hf_ndps_socket, tvb, foffset+10, 2, FALSE);
        break;
    case 0x00000001:
        proto_tree_add_item(ndps_tree, hf_ndps_port, tvb, foffset, 2, FALSE);
        proto_tree_add_item(ndps_tree, hf_ndps_ip, tvb, foffset+2, 4, FALSE);
        break;
    default:
        break;
    }
    tvb_ensure_bytes_exist(tvb, foffset, address_len);
    foffset += address_len;
    return foffset+(address_len%4);
}
