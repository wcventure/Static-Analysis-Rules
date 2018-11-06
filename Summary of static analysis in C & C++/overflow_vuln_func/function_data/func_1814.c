static int
address_item(tvbuff_t* tvb, proto_tree *ndps_tree, int foffset)
{
    guint32     address_type=0;

    address_type = tvb_get_ntohl(tvb, foffset); 
    proto_tree_add_uint(ndps_tree, hf_address_type, tvb, foffset, 4, address_type);
    foffset += 4;
    switch(address_type)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        foffset = qualifiedname(tvb, ndps_tree, foffset);
        break;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        foffset = ndps_string(tvb, hf_object_name, ndps_tree, foffset, NULL, 0);
        break;
    case 13:
        proto_tree_add_item(ndps_tree, hf_ndps_attrib_boolean, tvb, foffset, 4, FALSE);
        foffset += 4;
        break;
    case 14:
        proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, FALSE);
        foffset += 4;
        break;
    case 15:
        foffset = print_address(tvb, ndps_tree, foffset);
        break;
    case 16:
    case 17:
    default:
        foffset = ndps_string(tvb, hf_object_name, ndps_tree, foffset, NULL, 0);
        break;
    }
    return foffset;
}
