static int
objectidentification(tvbuff_t* tvb, proto_tree *ndps_tree, int foffset)
{
    guint32     object_type=0;
    proto_tree  *atree;
    proto_item  *aitem;
      
    object_type = tvb_get_ntohl(tvb, foffset); 
    aitem = proto_tree_add_item(ndps_tree, hf_obj_id_type, tvb, foffset, 4, FALSE);
    atree = proto_item_add_subtree(aitem, ett_ndps);
    foffset += 4;
    switch(object_type)
    {
        case 0:         /
            foffset = ndps_string(tvb, hf_printer_name, atree, foffset, NULL, 0);
            proto_tree_add_item(atree, hf_ndps_object, tvb, foffset, 
            4, FALSE);
            foffset += 4;
            break;
        case 1:         /
            foffset = ndps_string(tvb, hf_printer_name, atree, foffset, NULL, 0);
            proto_tree_add_item(atree, hf_ndps_document_number, tvb, foffset, 
            4, FALSE);
            foffset += 4;
            break;
        case 2:         /
            foffset = objectidentifier(tvb, atree, foffset);
            break;
        case 3:         /
            foffset = ndps_string(tvb, hf_object_name, atree, foffset, NULL, 0);
            if (foffset > tvb_length_remaining(tvb, foffset)) {
                return foffset;
            }
            foffset = name_or_id(tvb, atree, foffset);
            break;
        case 4:         /
            foffset = name_or_id(tvb, atree, foffset);
            break;
        case 5:         /
            foffset = ndps_string(tvb, hf_object_name, atree, foffset, NULL, 0);
            break;
        case 6:         /
            foffset = ndps_string(tvb, hf_printer_name, atree, foffset, NULL, 0);
            break;
        case 7:         /
            foffset = qualifiedname(tvb, ndps_tree, foffset);
            break;
        case 8:         /
            foffset = ndps_string(tvb, hf_object_name, atree, foffset, NULL, 0);
            foffset = objectidentifier(tvb, atree, foffset);
            proto_tree_add_item(atree, hf_ndps_event_type, tvb, foffset, 
            4, FALSE);
            foffset += 4;
        default:
            break;
    }
    return foffset;
}
