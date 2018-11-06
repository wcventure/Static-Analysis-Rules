static int
objectidentifier(tvbuff_t* tvb, proto_tree *ndps_tree, int foffset)
{
    guint32     length;
    const char  *label=NULL;
    guint32     label_value=0;
    proto_tree  *atree;
    proto_item  *aitem;
    gboolean    found=TRUE;

    length = tvb_get_ntohl(tvb, foffset);
    if (length==0)
    {
        return foffset;
    }
    if (ndps_show_oids)
    {
        proto_tree_add_uint(ndps_tree, hf_oid_struct_size, tvb, foffset, 4, length);
    }
    foffset += 4;
    switch (length)
    {
    case 9:
        label_value = tvb_get_ntohl(tvb, foffset+5);
        label = try_val_to_str(label_value, object_ids_7);
        if (label==NULL)
        {
            label = wmem_strdup(wmem_packet_scope(), "Unknown ID");
            found=FALSE;
        }
        aitem = proto_tree_add_uint_format(ndps_tree, hf_ndps_object_ids_7, tvb, foffset, length, label_value, "%s", label);
        break;
    case 10:
        label_value = tvb_get_ntohl(tvb, foffset+6);
        label = try_val_to_str(label_value, object_ids_8);
        if (label==NULL)
        {
            label = wmem_strdup(wmem_packet_scope(), "Unknown ID");
            found=FALSE;
        }
        aitem = proto_tree_add_uint_format(ndps_tree, hf_ndps_object_ids_8, tvb, foffset, length, label_value, "%s", label);
        break;
    case 11:
        label_value = tvb_get_ntohl(tvb, foffset+7);
        label = try_val_to_str(label_value, object_ids_9);
        if (label==NULL)
        {
            label = wmem_strdup(wmem_packet_scope(), "Unknown ID");
            found=FALSE;
        }
        aitem = proto_tree_add_uint_format(ndps_tree, hf_ndps_object_ids_9, tvb, foffset, length, label_value, "%s", label);
        break;
    case 12:
        label_value = tvb_get_ntohl(tvb, foffset+8);
        label = try_val_to_str(label_value, object_ids_10);
        if (label==NULL)
        {
            label = wmem_strdup(wmem_packet_scope(), "Unknown ID");
            found=FALSE;
        }
        aitem = proto_tree_add_uint_format(ndps_tree, hf_ndps_object_ids_10, tvb, foffset, length, label_value, "%s", label);
        break;
    case 13:
        label_value = tvb_get_ntohl(tvb, foffset+9);
        label = try_val_to_str(label_value, object_ids_11);
        if (label==NULL)
        {
            label = wmem_strdup(wmem_packet_scope(), "Unknown ID");
            found=FALSE;
        }
        aitem = proto_tree_add_uint_format(ndps_tree, hf_ndps_object_ids_11, tvb, foffset, length, label_value, "%s", label);
        break;
    case 14:
        label_value = tvb_get_ntohl(tvb, foffset+10);
        label = try_val_to_str(label_value, object_ids_12);
        if (label==NULL)
        {
            label = wmem_strdup(wmem_packet_scope(), "Unknown ID");
            found=FALSE;
        }
        aitem = proto_tree_add_uint_format(ndps_tree, hf_ndps_object_ids_12, tvb, foffset, length, label_value, "%s", label);
        break;
    case 15:
        label_value = tvb_get_ntohl(tvb, foffset+11);
        label = try_val_to_str(label_value, object_ids_13);
        if (label==NULL)
        {
            label = wmem_strdup(wmem_packet_scope(), "Unknown ID");
            found=FALSE;
        }
        aitem = proto_tree_add_uint_format(ndps_tree, hf_ndps_object_ids_13, tvb, foffset, length, label_value, "%s", label);
        break;
    case 16:
        label_value = tvb_get_ntohl(tvb, foffset+12);
        label = try_val_to_str(label_value, object_ids_14);
        if (label==NULL)
        {
            label = wmem_strdup(wmem_packet_scope(), "Unknown ID");
            found=FALSE;
        }
        aitem = proto_tree_add_uint_format(ndps_tree, hf_ndps_object_ids_14, tvb, foffset, length, label_value, "%s", label);
        break;
    case 17:
        label_value = tvb_get_ntohl(tvb, foffset+13);
        label = try_val_to_str(label_value, object_ids_15);
        if (label==NULL)
        {
            label = wmem_strdup(wmem_packet_scope(), "Unknown ID");
            found=FALSE;
        }
        aitem = proto_tree_add_uint_format(ndps_tree, hf_ndps_object_ids_15, tvb, foffset, length, label_value, "%s", label);
        break;
    case 18:
        label_value = tvb_get_ntohl(tvb, foffset+14);
        label = try_val_to_str(label_value, object_ids_16);
        if (label==NULL)
        {
            label = wmem_strdup(wmem_packet_scope(), "Unknown ID");
            found=FALSE;
        }
        aitem = proto_tree_add_uint_format(ndps_tree, hf_ndps_object_ids_16, tvb, foffset, length, label_value, "%s", label);
        break;
    default:
        aitem = proto_tree_add_uint_format(ndps_tree, hf_ndps_object_ids_7, tvb, foffset, length, 1, "Unknown ID");
        found=FALSE;
        break;
    }
    if (!found)
    {
        label_value = 1;
        label = try_val_to_str(label_value, object_ids_7);
    }
    if (ndps_show_oids)
    {
        atree = proto_item_add_subtree(aitem, ett_ndps);
        proto_tree_add_item(atree, hf_oid_asn1_type, tvb, foffset, 1, ENC_BIG_ENDIAN);
        foffset += 1;
        length = tvb_get_guint8(tvb, foffset);
        foffset += 1;
        proto_tree_add_item(atree, hf_ndps_oid, tvb, foffset, length, ENC_NA);
        foffset += length;
    }
    else
    {
        if (!found)
        {
            tvb_ensure_bytes_exist(tvb, foffset, length);
            foffset += length;
        }
        else
        {
            foffset += 1;
            length = tvb_get_guint8(tvb, foffset);
            foffset += 1;
            tvb_ensure_bytes_exist(tvb, foffset, length);
            foffset += length;
        }
    }
    global_attribute_name = label;
    /
    if ((int) (foffset+(length%2)) < 0) {
        THROW(ReportedBoundsError);
    }
    return foffset+(length%2);
}
