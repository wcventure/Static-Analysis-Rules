static guint
fObjectIdentifier (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint8      tag_no, tag_info;
    guint32     lvt;
    guint       tag_length;
    proto_item *ti;
    proto_tree *subtree;
    guint32     object_id;

    tag_length  = fTagHeader(tvb, offset, &tag_no, &tag_info, &lvt);
    object_id   = tvb_get_ntohl(tvb,offset+tag_length);
    object_type = object_id_type(object_id);
    ti = proto_tree_add_text(tree, tvb, offset, tag_length + 4,
        "ObjectIdentifier: %s, %u",
        val_to_split_str(object_type,
            128,
            BACnetObjectType,
            ASHRAE_Reserved_Fmt,
            Vendor_Proprietary_Fmt),
        object_id_instance(object_id));
    if (col_get_writable(pinfo->cinfo))
        col_append_fstr(pinfo->cinfo, COL_INFO, "%s,%u ",
            val_to_split_str(object_type,
                128,
                BACnetObjectType,
                ASHRAE_Reserved_Fmt,
                Vendor_Proprietary_Fmt),
                object_id_instance(object_id));

    /
    updateBacnetInfoValue(BACINFO_OBJECTID,
                  ep_strdup(val_to_split_str(object_type, 128,
                    BACnetObjectType, ASHRAE_Reserved_Fmt,
                    Vendor_Proprietary_Fmt)));
    updateBacnetInfoValue(BACINFO_INSTANCEID, ep_strdup_printf("Instance ID: %u",
                  object_id_instance(object_id)));

    /
    subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
    fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
    offset += tag_length;
    proto_tree_add_item(subtree, hf_bacapp_objectType, tvb, offset, 4, ENC_BIG_ENDIAN);
    proto_tree_add_item(subtree, hf_bacapp_instanceNumber, tvb, offset, 4, ENC_BIG_ENDIAN);
    offset += 4;

    return offset;
}
