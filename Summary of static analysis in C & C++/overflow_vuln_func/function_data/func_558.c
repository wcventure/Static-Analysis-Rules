static int
attribute_value(tvbuff_t* tvb, packet_info* pinfo, proto_tree *ndps_tree, int foffset)
{
    guint32     ii;
    guint32     jj;
    guint32     number_of_items;
    guint32     number_of_items2;
    guint32     attribute_type;
    guint32     integer_or_oid;
    guint32     event_object_type;
    guint32     ignored_type;
    guint32     resource_type;
    guint32     identifier_type;
    guint32     criterion_type;
    guint32     card_enum_time;
    guint32     media_type;
    guint32     doc_content;
    guint32     page_size;
    guint32     medium_size;
    guint32     numbers_up;
    guint32     colorant_set;
    guint32     length;
    guint32     dimension;
    guint32     location;
    guint32     cardinal;
    const char  *label;
    guint32     label_value;
    proto_tree  *atree, *btree;
    proto_item  *aitem, *bitem, *expert_item;

    if (global_attribute_name==NULL)
    {
        label_value = 1;
        label = try_val_to_str(label_value, object_ids_7);
        global_attribute_name = label;
    }
    attribute_type = tvb_get_ntohl(tvb, foffset);
    if (ndps_show_oids)
    {
        proto_tree_add_item(ndps_tree, hf_ndps_obj_attribute_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
    }
    foffset += 4;
    switch(attribute_type)
    {
        case 0:         /
            proto_tree_add_item(ndps_tree, hf_ndps_data, tvb, foffset+4, tvb_get_ntohl(tvb, foffset), ENC_NA);
            break;
        case 1:         /
        case 2:         /
        case 3:         /
        case 6:         /
        case 40:         /
        case 50:         /
        case 58:         /
        case 102:         /
        case 103:         /
        case 108:         /
            foffset = ndps_string(tvb, hf_object_name, ndps_tree, foffset, NULL);
            break;
        case 4:         /
        case 5:         /
        case 38:         /
            foffset = name_or_id(tvb, ndps_tree, foffset);
            break;
        case 39:         /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Item %d", ii+1);
                foffset = name_or_id(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 7:         /
        case 79:         /
            foffset = ndps_string(tvb, hf_object_name, ndps_tree, foffset, NULL);
            foffset = name_or_id(tvb, ndps_tree, foffset);
            break;
        case 8:         /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Name %u", ii+1);
                foffset = ndps_string(tvb, hf_object_name, atree, foffset, NULL);
                foffset = name_or_id(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 9:          /
        case 10:         /
        case 11:         /
        case 13:         /
        case 15:         /
        case 18:         /
        case 19:         /
        case 35:         /
        case 57:         /
        case 72:         /
        case 95:         /
            if (global_attribute_name != NULL &&
                strcmp(global_attribute_name,"(Novell) Attribute PRINTER SECURITY LEVEL")==0)
            {
                proto_tree_add_item(ndps_tree, hf_print_security, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
            }
            else
            {
                proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
            }
            break;
        case 12:         /
        case 14:         /
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length==4)
            {
                proto_tree_add_item(ndps_tree, hf_info_int32, tvb, foffset, length, ENC_BIG_ENDIAN);
            }
            tvb_ensure_bytes_exist(tvb, foffset, length);
            foffset += length;
            break;
        case 16:         /
        case 17:         /
            proto_tree_add_item(ndps_tree, hf_ndps_lower_range, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_upper_range, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 20:         /
        case 22:         /
        case 24:         /
        case 31:         /
        case 29:         /
            proto_tree_add_item(ndps_tree, hf_ndps_n64, tvb, foffset, 8, ENC_NA);
            foffset += 8;
            break;
        case 21:         /
        case 23:         /
        case 30:         /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Item %u", ii+1);
                proto_tree_add_item(atree, hf_ndps_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 25:         /
        case 26:         /
        case 32:         /
        case 33:         /
            proto_tree_add_item(ndps_tree, hf_ndps_lower_range_n64, tvb, foffset, 8, ENC_NA);
            foffset += 8;
            proto_tree_add_item(ndps_tree, hf_ndps_upper_range_n64, tvb, foffset, 8, ENC_NA);
            foffset += 8;
            break;
        case 27:         /
            proto_tree_add_item(ndps_tree, hf_ndps_lower_range_n64, tvb, foffset, 8, ENC_NA);
            foffset += 8;
            break;
        case 28:         /
            proto_tree_add_item(ndps_tree, hf_ndps_upper_range_n64, tvb, foffset, 8, ENC_NA);
            foffset += 8;
            break;
        case 34:         /
            proto_tree_add_item(ndps_tree, hf_ndps_attrib_boolean, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 36:         /
            foffset = objectidentifier(tvb, ndps_tree, foffset);
            break;
        case 37:         /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Object %d", ii+1);
                foffset = objectidentifier(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 41:         /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_names, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Name %u", ii+1);
                foffset = ndps_string(tvb, hf_object_name, atree, foffset, NULL);
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 42:         /
            proto_tree_add_item(ndps_tree, hf_ndps_realization, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 43:         /
            proto_tree_add_item(ndps_tree, hf_ndps_xdimension_n64, tvb, foffset, 8, ENC_NA);
            foffset += 8;
            proto_tree_add_item(ndps_tree, hf_ndps_ydimension_n64, tvb, foffset, 8, ENC_NA);
            foffset += 8;
            break;
        case 44:         /
            dimension = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_dim_value, tvb, foffset, 4, dimension);
            foffset += 4;
            if (dimension == 0) {
                proto_tree_add_item(ndps_tree, hf_ndps_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
            }
            else
            {
                foffset = name_or_id(tvb, ndps_tree, foffset);
            }
            proto_tree_add_item(ndps_tree, hf_ndps_dim_flag, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_n64, tvb, foffset, 8, ENC_NA);
            foffset += 8;
            break;
        case 45:         /
            dimension = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_xydim_value, tvb, foffset, 4, dimension);
            foffset += 4;
            if (dimension == 1) {
                foffset = name_or_id(tvb, ndps_tree, foffset);
            }
            else
            {
                proto_tree_add_item(ndps_tree, hf_ndps_xdimension_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(ndps_tree, hf_ndps_ydimension_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
            }
            proto_tree_add_item(ndps_tree, hf_ndps_dim_flag, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_n64, tvb, foffset, 8, ENC_NA);
            foffset += 8;
            break;
        case 46:         /
            location = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_location_value, tvb, foffset, 4, location);
            foffset += 4;
            if (location == 0) {
                number_of_items = tvb_get_ntohl(tvb, foffset);
                expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_locations, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (ii = 0; ii < number_of_items; ii++ )
                {
                    if (ii >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                        break;
                    }
                    atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Location %u", ii+1);
                    proto_tree_add_item(atree, hf_ndps_n64, tvb, foffset, 8, ENC_NA);
                    foffset += 8;
                    proto_item_set_end(aitem, tvb, foffset);
                }
            }
            else
            {
                foffset = name_or_id(tvb, ndps_tree, foffset);
            }
            proto_tree_add_item(ndps_tree, hf_ndps_dim_flag, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_n64, tvb, foffset, 8, ENC_NA);
            foffset += 8;
            break;
        case 47:         /
            proto_tree_add_item(ndps_tree, hf_ndps_xmin_n64, tvb, foffset, 8, ENC_NA);
            foffset += 8;
            proto_tree_add_item(ndps_tree, hf_ndps_xmax_n64, tvb, foffset, 8, ENC_NA);
            foffset += 8;
            proto_tree_add_item(ndps_tree, hf_ndps_ymin_n64, tvb, foffset, 8, ENC_NA);
            foffset += 8;
            proto_tree_add_item(ndps_tree, hf_ndps_ymax_n64, tvb, foffset, 8, ENC_NA);
            foffset += 8;
            break;
        case 48:         /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_areas, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Area %u", ii+1);
                proto_tree_add_item(atree, hf_ndps_xmin_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(atree, hf_ndps_xmax_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(atree, hf_ndps_ymin_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(atree, hf_ndps_ymax_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 49:         /
            proto_tree_add_item(ndps_tree, hf_ndps_edge_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 51:         /
            cardinal = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_cardinal_or_oid, tvb, foffset, 4, cardinal);
            foffset += 4;
            if (cardinal==0) {
                proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
            }
            else
            {
                foffset = objectidentifier(tvb, ndps_tree, foffset);
            }
            break;
        case 52:         /
            foffset = objectidentifier(tvb, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 53:         /
            cardinal = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_cardinal_name_or_oid, tvb, foffset, 4, cardinal);
            foffset += 4;
            if (cardinal==0) {
                proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
            }
            else
            {
                foffset = name_or_id(tvb, ndps_tree, foffset);
            }
            break;
        case 54:         /
            integer_or_oid = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_integer_or_oid, tvb, foffset, 4, integer_or_oid);
            foffset += 4;
            if (integer_or_oid==0) {
                foffset = objectidentifier(tvb, ndps_tree, foffset);
            }
            else
            {
                proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
            }
            break;
        case 55:         /
            proto_tree_add_item(ndps_tree, hf_ndps_profile_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_persistence, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = qualifiedname(tvb, ndps_tree, foffset);
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length==4)
            {
                proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, length, ENC_BIG_ENDIAN);
            }
            foffset += length;
            proto_tree_add_item(ndps_tree, hf_ndps_language_id, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = name_or_id(tvb, ndps_tree, foffset);

            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_address_items, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Address Item %u", ii+1);
                foffset = address_item(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
            }
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_events, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Event %u", ii+1);
                proto_tree_add_item(atree, hf_ndps_event_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                foffset = objectidentifier(tvb, atree, foffset);
                foffset += align_4(tvb, foffset);
                foffset = objectidentification(tvb, atree, foffset);
                proto_tree_add_item(atree, hf_ndps_object_op, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                event_object_type = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_event_object_identifier, tvb, foffset, 4, event_object_type);
                foffset += 4;
                switch (event_object_type)
                {
                    case 2:
                        /
                        number_of_items2 = tvb_get_ntohl(tvb, foffset);
                        expert_item = proto_tree_add_uint(atree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items2);
                        foffset += 4;
                        for (jj = 0; jj < number_of_items2; jj++ )
                        {
                            if (jj >= NDPS_MAX_ITEMS) {
                                expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                                break;
                            }
                            foffset = objectidentifier(tvb, atree, foffset);
                        }
                        foffset += 4;
                        break;

                    case 1:
                        foffset = objectidentifier(tvb, atree, foffset);
                        break;

                    case 0:
                        number_of_items2 = tvb_get_ntohl(tvb, foffset);
                        expert_item = proto_tree_add_uint(atree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items2);
                        foffset += 4;
                        for (jj = 0; jj < number_of_items2; jj++ )
                        {
                            if (jj >= NDPS_MAX_ITEMS) {
                                expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                                break;
                            }
                            btree = proto_tree_add_subtree_format(atree, tvb, foffset, -1, ett_ndps, &bitem, "Object %u", jj+1);
                            foffset = objectidentifier(tvb, btree, foffset);
                            proto_item_set_end(bitem, tvb, foffset);
                        }
                        break;
                }
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 56:         /
        case 63:         /
        case 66:         /
            length = tvb_get_ntohl(tvb, foffset);
            ndps_string(tvb, hf_info_string, ndps_tree, foffset, NULL);
            foffset += length+2;
            foffset += align_4(tvb, foffset);
            break;
        case 59:         /
            proto_tree_add_item(ndps_tree, hf_ndps_delivery_add_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
            event_object_type = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            switch(event_object_type)
            {
                case 0:     /
                case 1:     /
                case 2:     /
                case 3:     /
                    foffset = ndps_string(tvb, hf_info_string, ndps_tree, foffset, NULL);
                    break;
                case 4:     /
                    foffset = ndps_string(tvb, hf_object_name, ndps_tree, foffset, NULL);
                    foffset = name_or_id(tvb, ndps_tree, foffset);
                    break;
                case 5:     /
                case 6:     /
                    foffset = objectidentifier(tvb, ndps_tree, foffset);
                    foffset = qualifiedname(tvb, ndps_tree, foffset);
                    break;
                default:
                    break;
            }
            break;
        case 60:         /
            foffset = objectidentification(tvb, ndps_tree, foffset);
            break;
        case 61:         /
            foffset = objectidentifier(tvb, ndps_tree, foffset);
            foffset = name_or_id(tvb, ndps_tree, foffset);
            foffset = address_item(tvb, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = name_or_id(tvb, ndps_tree, foffset);
            break;
        case 62:         /
            foffset = objectidentifier(tvb, ndps_tree, foffset);
            criterion_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_criterion_type, tvb, foffset, 4, criterion_type);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 64:         /
            foffset = objectidentifier(tvb, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 65:         /
            foffset = objectidentifier(tvb, ndps_tree, foffset);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_job_categories, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Job %u", ii+1);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(atree, hf_ndps_attribute_value, tvb, foffset, length, ENC_BIG_ENDIAN);
                }
                foffset += length;
                foffset += (length%2);
                if ((int) foffset <= 0)
                    THROW(ReportedBoundsError);
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 67:         /
            proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = objectidentifier(tvb, ndps_tree, foffset);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_ignored_attributes, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Ignored Attribute %u", ii+1);
                ignored_type = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_ignored_type, tvb, foffset, 4, ignored_type);
                foffset += 4;
                if (ignored_type == 38)
                {
                    foffset = name_or_id(tvb, atree, foffset);
                }
                else
                {
                    foffset = objectidentifier(tvb, atree, foffset);
                }
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 68:         /
            resource_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_resource_type, tvb, foffset, 4, resource_type);
            foffset += 4;
            if (resource_type == 0)
            {
                foffset = name_or_id(tvb, ndps_tree, foffset);
            }
            else
            {
                foffset = ndps_string(tvb, hf_ndps_tree, ndps_tree, foffset, NULL);
            }
            break;
        case 69:         /
            foffset = name_or_id(tvb, ndps_tree, foffset);
            foffset = name_or_id(tvb, ndps_tree, foffset);
            break;
        case 70:         /
            foffset = ndps_string(tvb, hf_ndps_tree, ndps_tree, foffset, NULL);
            foffset = ndps_string(tvb, hf_ndps_tree, ndps_tree, foffset, NULL);
            break;
        case 71:         /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_resources, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Resource %u", ii+1);
                resource_type = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_resource_type, tvb, foffset, 4, resource_type);
                foffset += 4;
                if (resource_type == 0)
                {
                    foffset = name_or_id(tvb, atree, foffset);
                }
                else
                {
                    foffset = ndps_string(tvb, hf_ndps_tree, atree, foffset, NULL);
                }
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 73:         /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_page_selects, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Page Select %u", ii+1);
                proto_tree_add_item(atree, hf_ndps_page_flag, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                identifier_type = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_identifier_type, tvb, foffset, 4, identifier_type);
                foffset += 4;
                if (identifier_type == 0)
                {
                    proto_tree_add_item(atree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                }
                if (identifier_type == 1)
                {
                    foffset = ndps_string(tvb, hf_ndps_tree, atree, foffset, NULL);
                }
                if (identifier_type == 2)
                {
                    foffset = name_or_id(tvb, atree, foffset);
                }
                proto_tree_add_item(atree, hf_ndps_page_flag, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                identifier_type = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(atree, hf_ndps_identifier_type, tvb, foffset, 4, identifier_type);
                foffset += 4;
                if (identifier_type == 0)
                {
                    proto_tree_add_item(atree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                }
                if (identifier_type == 1)
                {
                    foffset = ndps_string(tvb, hf_ndps_tree, atree, foffset, NULL);
                }
                if (identifier_type == 2)
                {
                    foffset = name_or_id(tvb, atree, foffset);
                }
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 74:         /
            media_type = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_media_type, tvb, foffset, 4, media_type);
            foffset += 4;
            if (media_type == 0)
            {
                foffset = name_or_id(tvb, ndps_tree, foffset);
            }
            else
            {
                foffset = ndps_string(tvb, hf_ndps_tree, ndps_tree, foffset, NULL);
                number_of_items = tvb_get_ntohl(tvb, foffset);
                expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_item_count, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (ii = 0; ii < number_of_items; ii++ )
                {
                    if (ii >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                        break;
                    }
                    atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Item %u", ii+1);
                    proto_tree_add_item(atree, hf_ndps_page_flag, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                    identifier_type = tvb_get_ntohl(tvb, foffset);
                    proto_tree_add_uint(atree, hf_ndps_identifier_type, tvb, foffset, 4, identifier_type);
                    foffset += 4;
                    if (identifier_type == 0)
                    {
                        proto_tree_add_item(atree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                        foffset += 4;
                    }
                    if (identifier_type == 1)
                    {
                        foffset = ndps_string(tvb, hf_ndps_tree, atree, foffset, NULL);
                    }
                    if (identifier_type == 2)
                    {
                        foffset = name_or_id(tvb, atree, foffset);
                    }
                    proto_item_set_end(aitem, tvb, foffset);
                }
            }
            break;
        case 75:         /
            doc_content = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_doc_content, tvb, foffset, 4, doc_content);
            foffset += 4;
            if (doc_content == 0)
            {
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length!=0)
                {
                    proto_tree_add_item(ndps_tree, hf_ndps_octet_string, tvb, foffset, length, ENC_NA);
                }
                foffset += length;
                foffset += (length%2);
                if ((int) foffset <= 0)
                    THROW(ReportedBoundsError);
            }
            else
            {
                foffset = ndps_string(tvb, hf_object_name, ndps_tree, foffset, NULL);
                foffset = name_or_id(tvb, ndps_tree, foffset);
            }
            break;
        case 76:         /
            page_size = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_page_size, tvb, foffset, 4, page_size);
            foffset += 4;
            if (page_size == 0)
            {
                foffset = objectidentifier(tvb, ndps_tree, foffset);
            }
            else
            {
                proto_tree_add_item(ndps_tree, hf_ndps_xdimension_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(ndps_tree, hf_ndps_ydimension_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
            }
            break;
        case 77:         /
            proto_tree_add_item(ndps_tree, hf_ndps_direction, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 78:         /
            proto_tree_add_item(ndps_tree, hf_ndps_page_order, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 80:         /
            foffset = name_or_id(tvb, ndps_tree, foffset);
            medium_size = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_medium_size, tvb, foffset, 4, medium_size);
            foffset += 4;
            if (medium_size == 0)
            {
                page_size = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_uint(ndps_tree, hf_ndps_page_size, tvb, foffset, 4, page_size);
                foffset += 4;
                if (page_size == 0)
                {
                    foffset = objectidentifier(tvb, ndps_tree, foffset);
                }
                else
                {
                    proto_tree_add_item(ndps_tree, hf_ndps_xdimension_n64, tvb, foffset, 8, ENC_NA);
                    foffset += 8;
                    proto_tree_add_item(ndps_tree, hf_ndps_ydimension_n64, tvb, foffset, 8, ENC_NA);
                    foffset += 8;
                }
                proto_tree_add_item(ndps_tree, hf_ndps_long_edge_feeds, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_tree_add_item(ndps_tree, hf_ndps_xmin_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(ndps_tree, hf_ndps_xmax_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(ndps_tree, hf_ndps_ymin_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(ndps_tree, hf_ndps_ymax_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
            }
            else
            {
                proto_tree_add_item(ndps_tree, hf_ndps_lower_range_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(ndps_tree, hf_ndps_upper_range_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(ndps_tree, hf_ndps_inc_across_feed, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(ndps_tree, hf_ndps_lower_range_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(ndps_tree, hf_ndps_upper_range_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(ndps_tree, hf_ndps_size_inc_in_feed, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(ndps_tree, hf_ndps_long_edge_feeds, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_tree_add_item(ndps_tree, hf_ndps_xmin_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(ndps_tree, hf_ndps_xmax_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(ndps_tree, hf_ndps_ymin_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
                proto_tree_add_item(ndps_tree, hf_ndps_ymax_n64, tvb, foffset, 8, ENC_NA);
                foffset += 8;
            }
            break;
        case 81:         /
            foffset = name_or_id(tvb, ndps_tree, foffset);
            foffset = name_or_id(tvb, ndps_tree, foffset);
            break;
        case 82:         /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_page_informations, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Page Information %u", ii+1);
                proto_tree_add_item(atree, hf_ndps_page_order, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_tree_add_item(atree, hf_ndps_page_orientation, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 83:         /
            proto_tree_add_item(ndps_tree, hf_ndps_identifier_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 84:         /
            foffset = objectidentifier(tvb, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_lower_range, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_upper_range, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 85:         /
            foffset = objectidentifier(tvb, ndps_tree, foffset);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_categories, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Category %u", ii+1);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(atree, hf_ndps_attribute_value, tvb, foffset, length, ENC_BIG_ENDIAN);
                }
                foffset += length;
                foffset += (length%2);
                if ((int) foffset <= 0)
                    THROW(ReportedBoundsError);
            }
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_values, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Value %d", ii+1);
                length = tvb_get_ntohl(tvb, foffset);
                foffset += 4;
                if (length==4)
                {
                    proto_tree_add_item(atree, hf_ndps_attribute_value, tvb, foffset, length, ENC_BIG_ENDIAN);
                }
                foffset += length;
                foffset += (length%2);
                if ((int) foffset <= 0)
                    THROW(ReportedBoundsError);
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 86:         /
            numbers_up=tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_numbers_up, tvb, foffset, 4, numbers_up);
            foffset += 4;
            switch(numbers_up)
            {
            case 0:     /
                proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                break;
            case 1:     /
                foffset = name_or_id(tvb, ndps_tree, foffset);
                break;
            case 2:     /
                proto_tree_add_item(ndps_tree, hf_ndps_lower_range, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_tree_add_item(ndps_tree, hf_ndps_upper_range, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                break;
            default:
                break;
            }
            break;
        case 87:         /
        case 88:         /
            foffset = ndps_string(tvb, hf_object_name, ndps_tree, foffset, NULL);
            proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 89:         /
            foffset = ndps_string(tvb, hf_object_name, ndps_tree, foffset, NULL);
            foffset = qualifiedname(tvb, ndps_tree, foffset);
            break;
        case 90:         /
            foffset = ndps_string(tvb, hf_object_name, ndps_tree, foffset, NULL);
            proto_tree_add_item(ndps_tree, hf_level, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_interval, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 91:         /
            proto_tree_add_item(ndps_tree, hf_ndps_address, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length!=0)
            {
                proto_tree_add_item(ndps_tree, hf_ndps_add_bytes, tvb, foffset, 4, ENC_NA);
            }
            foffset += length;
            break;
        case 92:         /
            dimension = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_uint(ndps_tree, hf_ndps_xydim_value, tvb, foffset, 4, dimension);
            foffset += 4;
            switch (dimension)
            {
                case 1:
                    foffset = name_or_id(tvb, ndps_tree, foffset);
                    break;

                case 0:
                    proto_tree_add_item(ndps_tree, hf_ndps_xdimension_n64, tvb, foffset, 8, ENC_NA);
                    foffset += 8;
                    proto_tree_add_item(ndps_tree, hf_ndps_ydimension_n64, tvb, foffset, 8, ENC_NA);
                    foffset += 8;
                    break;

                default:
                    proto_tree_add_item(ndps_tree, hf_ndps_xdimension, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                    proto_tree_add_item(ndps_tree, hf_ndps_ydimension, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                    break;
            }
            break;
        case 93:         /
            foffset = name_or_id(tvb, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_xdimension, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_ydimension, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 94:         /
            foffset += 4;
            foffset = name_or_id(tvb, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_state_severity, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_training, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = objectidentifier(tvb, ndps_tree, foffset);
            foffset += align_4(tvb, foffset);
            foffset = objectidentification(tvb, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            number_of_items = tvb_get_ntohl(tvb, foffset);
            foffset += 4*number_of_items;
            /
            foffset = name_or_id(tvb, ndps_tree, foffset);

            break;
        case 96:         /
            foffset = qualifiedname(tvb, ndps_tree, foffset);
            break;
        case 97:         /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_names, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Name %u", ii+1);
                foffset = qualifiedname(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 98:         /
            colorant_set = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_colorant_set, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            if (colorant_set==0)
            {
                foffset = name_or_id(tvb, ndps_tree, foffset);
            }
            else
            {

                foffset = objectidentifier(tvb, ndps_tree, foffset);
                number_of_items = tvb_get_ntohl(tvb, foffset);
                expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_colorants, tvb, foffset, 4, number_of_items);
                foffset += 4;
                for (ii = 0; ii < number_of_items; ii++ )
                {
                    if (ii >= NDPS_MAX_ITEMS) {
                        expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                        break;
                    }
                    atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Colorant %u", ii+1);
                    foffset = name_or_id(tvb, atree, foffset);
                    proto_item_set_end(aitem, tvb, foffset);
                }
            }
            break;
        case 99:         /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_printer_def_count, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Printer %u", ii+1);
                foffset = ndps_string(tvb, hf_ndps_printer_type, atree, foffset, NULL);
                foffset = ndps_string(tvb, hf_ndps_printer_manuf, atree, foffset, NULL);
                foffset = ndps_string(tvb, hf_ndps_inf_file_name, atree, foffset, NULL);
                proto_item_set_end(aitem, tvb, foffset);
            }
            proto_tree_add_item(ndps_tree, hf_os_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 100:         /
            foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL);
            foffset = objectidentifier(tvb, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_event_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 101:         /
            foffset = qualifiedname(tvb, ndps_tree, foffset);
            foffset = qualifiedname(tvb, ndps_tree, foffset);
            break;
        case 104:         /
            card_enum_time = tvb_get_ntohl(tvb, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_card_enum_time, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            switch (card_enum_time)
            {
                case 0:
                    proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                    break;

                case 1:
                    proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                    break;

                default:
                    proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                    foffset += 4;
                    break;
            }
            break;
        case 105:         /
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Object %u", ii+1);
                foffset = ndps_string(tvb, hf_ndps_pa_name, atree, foffset, NULL);
                proto_tree_add_item(atree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_item_set_end(aitem, tvb, foffset);
            }
            break;
        case 106:         /
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length!=0)
            {
                proto_tree_add_item(ndps_tree, hf_ndps_octet_string, tvb, foffset, length, ENC_NA);
            }
            foffset += length;
            foffset += (length%2);
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length!=0)
            {
                proto_tree_add_item(ndps_tree, hf_ndps_octet_string, tvb, foffset, length, ENC_NA);
            }
            foffset += length;
            foffset += (length%2);
            if ((int) foffset <= 0)
                THROW(ReportedBoundsError);
            break;
        case 107:         /
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length!=0)
            {
                proto_tree_add_item(ndps_tree, hf_ndps_octet_string, tvb, foffset, length, ENC_NA);
            }
            foffset += length;
            foffset += (length%2);
            proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            break;
        case 109:         /
            proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_persistence, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = qualifiedname(tvb, ndps_tree, foffset);
            length = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            if (length!=0)
            {
                proto_tree_add_item(ndps_tree, hf_ndps_octet_string, tvb, foffset, length, ENC_NA);
            }
            foffset += length;
            foffset += (length%2);
            proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = name_or_id(tvb, ndps_tree, foffset);
            proto_tree_add_item(ndps_tree, hf_ndps_delivery_add_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
            event_object_type = tvb_get_ntohl(tvb, foffset);
            foffset += 4;
            switch(event_object_type)
            {
                case 0:     /
                case 1:     /
                case 2:     /
                    foffset = ndps_string(tvb, hf_ndps_tree, ndps_tree, foffset, NULL);
                    break;
                case 3:     /
                    length = tvb_get_ntohl(tvb, foffset);
                    foffset += 4;
                    if (length!=0)
                    {
                        proto_tree_add_item(ndps_tree, hf_ndps_octet_string, tvb, foffset, length, ENC_NA);
                    }
                    foffset += length;
                    foffset += (length%2);
                    if ((int) foffset <= 0)
                        THROW(ReportedBoundsError);
                    break;
                case 4:     /
                    foffset = ndps_string(tvb, hf_object_name, ndps_tree, foffset, NULL);
                    foffset = name_or_id(tvb, ndps_tree, foffset);
                    break;
                case 5:     /
                case 6:     /
                    foffset = objectidentifier(tvb, ndps_tree, foffset);
                    foffset = qualifiedname(tvb, ndps_tree, foffset);
                    break;
                default:
                    break;
            }
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_events, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Event %u", ii+1);
                foffset = ndps_string(tvb, hf_object_name, atree, foffset, NULL);
                foffset = objectidentifier(tvb, atree, foffset);
                proto_tree_add_item(atree, hf_ndps_event_type, tvb, foffset, 4, ENC_BIG_ENDIAN);
                foffset += 4;
                proto_item_set_end(aitem, tvb, foffset);
            }
            foffset = objectidentifier(tvb, ndps_tree, foffset);
            foffset = qualifiedname(tvb, ndps_tree, foffset);
            number_of_items = tvb_get_ntohl(tvb, foffset);
            expert_item = proto_tree_add_uint(ndps_tree, hf_ndps_num_objects, tvb, foffset, 4, number_of_items);
            foffset += 4;
            for (ii = 0; ii < number_of_items; ii++ )
            {
                if (ii >= NDPS_MAX_ITEMS) {
                    expert_add_info(pinfo, expert_item, &ei_ndps_truncated);
                    break;
                }
                atree = proto_tree_add_subtree_format(ndps_tree, tvb, foffset, -1, ett_ndps, &aitem, "Object %u", ii+1);
                foffset = objectidentifier(tvb, atree, foffset);
                proto_item_set_end(aitem, tvb, foffset);
            }
            proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_attribute_value, tvb, foffset, 4, ENC_BIG_ENDIAN);
            foffset += 4;
            foffset = ndps_string(tvb, hf_ndps_pa_name, ndps_tree, foffset, NULL);
            break;
        default:
            break;
    }
    return foffset;
}
