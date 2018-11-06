static int
dissect_ExpectedSubmoduleBlockReq_block(tvbuff_t *tvb, int offset,
    packet_info *pinfo, proto_tree *tree, proto_item *item, guint8 *drep, guint8 u8BlockVersionHigh, guint8 u8BlockVersionLow)
{
    guint16     u16NumberOfAPIs;
    guint32     u32Api;
    guint16     u16SlotNr;
    guint32     u32ModuleIdentNumber;
    guint16     u16ModuleProperties;
    guint16     u16NumberOfSubmodules;
    guint16     u16SubslotNr;
    guint32     u32SubmoduleIdentNumber;
    guint16     u16SubmoduleProperties;
    proto_item *api_item;
    proto_tree *api_tree;
    guint32     u32ApiStart;
    proto_item *sub_item;
    proto_tree *sub_tree;
    proto_item *submodule_item;
    proto_tree *submodule_tree;
    guint32     u32SubStart;

    /
    const char vendorIdStr[] = "VendorID=\"";
    const char deviceIdStr[] = "DeviceID=\"";
    const char moduleStr[] = "ModuleIdentNumber=\"";
    const char subModuleStr[] = "SubmoduleIdentNumber=\"";
    const char profisafeStr[] = "PROFIsafeSupported=\"true\"";
    const char fParameterStr[] = "<F_ParameterRecordDataItem";
    const char fParameterIndexStr[] = "Index=";
    const char moduleNameInfo[] = "<Name";
    const char moduleValueInfo[] = "Value=\"";

    guint16  searchVendorID = 0;
    guint16  searchDeviceID = 0;
    gboolean vendorMatch;
    gboolean deviceMatch;
    conversation_t *conversation;
    stationInfo    *station_info = NULL;
    ioDataObject   *io_data_object = NULL; /

    /
    guint32  read_vendor_id;
    guint32  read_device_id;
    guint32  read_module_id;
    guint32  read_submodule_id;
    gboolean gsdmlFoundFlag;
    gchar   tmp_moduletext[MAX_NAMELENGTH];
    gchar   *convertStr;      /
    gchar   *pch;             /
    gchar   *puffer;          /
    gchar   *temp;            /
    gchar   *diropen = NULL;  /
    GDir    *dir;
    FILE    *fp = NULL;       /
    const gchar *filename;    /

    /
    convertStr = (gchar*)wmem_alloc(wmem_packet_scope(), MAX_NAMELENGTH);
    convertStr[0] = '\0';
    pch = (gchar*)wmem_alloc(wmem_packet_scope(), MAX_LINE_LENGTH);
    pch[0] = '\0';
    puffer = (gchar*)wmem_alloc(wmem_packet_scope(), MAX_LINE_LENGTH);
    puffer[0] = '\0';
    temp = (gchar*)wmem_alloc(wmem_packet_scope(), MAX_LINE_LENGTH);
    temp[0] = '\0';

    /
    io_data_object = wmem_new0(wmem_file_scope(), ioDataObject);
    io_data_object->profisafeSupported = FALSE;
    io_data_object->moduleNameStr = wmem_strdup(wmem_file_scope(), "Unknown");
    vendorMatch = FALSE;
    deviceMatch = FALSE;
    gsdmlFoundFlag = FALSE;


    if (u8BlockVersionHigh != 1 || u8BlockVersionLow != 0) {
        expert_add_info_format(pinfo, item, &ei_pn_io_block_version,
            "Block version %u.%u not implemented yet!", u8BlockVersionHigh, u8BlockVersionLow);
        return offset;
    }

    offset = dissect_dcerpc_uint16(tvb, offset, pinfo, tree, drep,
                        hf_pn_io_number_of_apis, &u16NumberOfAPIs);

    proto_item_append_text(item, ": APIs:%u", u16NumberOfAPIs);


    /
    conversation = find_conversation(pinfo->num, &pinfo->dl_src, &pinfo->dl_dst, PT_NONE, 0, 0, 0);
    if (conversation == NULL) {
        conversation = conversation_new(pinfo->num, &pinfo->dl_src, &pinfo->dl_dst, PT_NONE, 0, 0, 0);
    }

    station_info = (stationInfo*)conversation_get_proto_data(conversation, proto_pn_dcp);
    if (station_info != NULL) {
        station_info->gsdFound = FALSE;
        station_info->gsdPathLength = FALSE;

        /
        searchVendorID = station_info->u16Vendor_id;
        searchDeviceID = station_info->u16Device_id;

        /
        if(pnio_ps_networkpath[0] != '\0') {   /
            station_info->gsdPathLength = TRUE;

            if ((dir = g_dir_open(pnio_ps_networkpath, 0, NULL)) != NULL) {
                /
                while ((filename = g_dir_read_name(dir)) != NULL) {

                    /
                    diropen = wmem_strdup_printf(wmem_packet_scope(), "%s" G_DIR_SEPARATOR_S "%s", pnio_ps_networkpath, filename);

                    /
                    fp = ws_fopen(diropen, "r");

                    if(fp != NULL) {
                        /
                        while(pn_fgets(puffer, MAX_LINE_LENGTH, fp) != NULL) {
                            /
                            if((strstr(puffer, vendorIdStr)) != NULL) {
                                memset (convertStr, 0, sizeof(*convertStr));
                                pch = strstr(puffer, vendorIdStr);
                                if (pch!= NULL && sscanf(pch, "VendorID=\"%[^\"]", convertStr) == 1) {
                                    read_vendor_id = (guint32) strtoul (convertStr, NULL, 0);

                                    if(read_vendor_id == searchVendorID) {
                                        vendorMatch = TRUE;        /
                                    }
                                }
                            }

                            /
                            if((strstr(puffer, deviceIdStr)) != NULL) {
                                memset(convertStr, 0, sizeof(*convertStr));
                                pch = strstr(puffer, deviceIdStr);
                                if (pch != NULL && sscanf(pch, "DeviceID=\"%[^\"]", convertStr) == 1) {
                                    read_device_id = (guint32)strtoul(convertStr, NULL, 0);

                                    if(read_device_id == searchDeviceID) {
                                        deviceMatch = TRUE;        /
                                    }
                                }
                            }
                        }

                        fclose(fp);
                        fp = NULL;

                        if(vendorMatch && deviceMatch) {
                            break;        /
                        }
                        else {
                            /
                            vendorMatch = FALSE;
                            deviceMatch = FALSE;
                            gsdmlFoundFlag = FALSE;
                            diropen = "";           /
                        }
                    }
                }

                g_dir_close(dir);
            }

            /
            if(vendorMatch && deviceMatch) {
                gsdmlFoundFlag = TRUE;
                station_info->gsdFound = TRUE;
                station_info->gsdLocation = wmem_strdup(wmem_file_scope(), diropen);
            }
            else {
                /
                station_info->gsdLocation = wmem_strdup_printf(wmem_file_scope(), "%s" G_DIR_SEPARATOR_S "*.xml", pnio_ps_networkpath);
            }
        }
        else {
            /
            station_info->gsdPathLength = FALSE;
        }
    }

    while (u16NumberOfAPIs--) {
        api_item = proto_tree_add_item(tree, hf_pn_io_api_tree, tvb, offset, 0, ENC_NA);
        api_tree = proto_item_add_subtree(api_item, ett_pn_io_api);
        u32ApiStart = offset;

        /
        offset = dissect_dcerpc_uint32(tvb, offset, pinfo, api_tree, drep,
                            hf_pn_io_api, &u32Api);
        /
        offset = dissect_dcerpc_uint16(tvb, offset, pinfo, api_tree, drep,
                            hf_pn_io_slot_nr, &u16SlotNr);
        /
        offset = dissect_dcerpc_uint32(tvb, offset, pinfo, api_tree, drep,
                            hf_pn_io_module_ident_number, &u32ModuleIdentNumber);
        /
        offset = dissect_dcerpc_uint16(tvb, offset, pinfo, api_tree, drep,
                            hf_pn_io_module_properties, &u16ModuleProperties);
        /
        offset = dissect_dcerpc_uint16(tvb, offset, pinfo, api_tree, drep,
                            hf_pn_io_number_of_submodules, &u16NumberOfSubmodules);

        proto_item_append_text(api_item, ": %u, Slot:0x%x, IdentNumber:0x%x Properties:0x%x Submodules:%u",
            u32Api, u16SlotNr, u32ModuleIdentNumber, u16ModuleProperties, u16NumberOfSubmodules);

        proto_item_append_text(item, ", Submodules:%u", u16NumberOfSubmodules);

        while (u16NumberOfSubmodules--) {
            sub_item = proto_tree_add_item(api_tree, hf_pn_io_submodule_tree, tvb, offset, 0, ENC_NA);
            sub_tree = proto_item_add_subtree(sub_item, ett_pn_io_submodule);
            u32SubStart = offset;

            /
            offset = dissect_dcerpc_uint16(tvb, offset, pinfo, sub_tree, drep,
                                hf_pn_io_subslot_nr, &u16SubslotNr);
            /
            offset = dissect_dcerpc_uint32(tvb, offset, pinfo, sub_tree, drep,
                            hf_pn_io_submodule_ident_number, &u32SubmoduleIdentNumber);
            /
            submodule_item = proto_tree_add_item(sub_tree, hf_pn_io_submodule_properties, tvb, offset, 2, ENC_BIG_ENDIAN);
            submodule_tree = proto_item_add_subtree(submodule_item, ett_pn_io_submodule_properties);
            dissect_dcerpc_uint16(tvb, offset, pinfo, submodule_tree, drep,
                            hf_pn_io_submodule_properties_reserved, &u16SubmoduleProperties);
            dissect_dcerpc_uint16(tvb, offset, pinfo, submodule_tree, drep,
                            hf_pn_io_submodule_properties_discard_ioxs, &u16SubmoduleProperties);
            dissect_dcerpc_uint16(tvb, offset, pinfo, submodule_tree, drep,
                            hf_pn_io_submodule_properties_reduce_output_submodule_data_length, &u16SubmoduleProperties);
            dissect_dcerpc_uint16(tvb, offset, pinfo, submodule_tree, drep,
                            hf_pn_io_submodule_properties_reduce_input_submodule_data_length, &u16SubmoduleProperties);
            dissect_dcerpc_uint16(tvb, offset, pinfo, submodule_tree, drep,
                            hf_pn_io_submodule_properties_shared_input, &u16SubmoduleProperties);
            offset = dissect_dcerpc_uint16(tvb, offset, pinfo, submodule_tree, drep,
                            hf_pn_io_submodule_properties_type, &u16SubmoduleProperties);

            io_data_object->slotNr = u16SlotNr;
            io_data_object->subSlotNr = u16SubslotNr;
            io_data_object->moduleIdentNr = u32ModuleIdentNumber;
            io_data_object->subModuleIdentNr = u32SubmoduleIdentNumber;
            io_data_object->discardIOXS = u16SubmoduleProperties & 0x0020;

            /

            io_data_object->amountInGSDML = 0;
            io_data_object->fParameterIndexNr = 0;
            io_data_object->profisafeSupported = FALSE;

            if (diropen != NULL) {
                fp = ws_fopen(diropen, "r");
            }
            else {
                fp = NULL;
            }
            if(fp != NULL && gsdmlFoundFlag) {
                fseek(fp, 0, SEEK_SET);

                /
                while(pn_fgets(temp, MAX_LINE_LENGTH, fp) != NULL) {
                    if((strstr(temp, fParameterStr)) != NULL) {
                        memset (convertStr, 0, sizeof(*convertStr));

                        pch = strstr(temp, fParameterIndexStr);
                        if (pch != NULL && sscanf(pch, "Index=\"%[^\"]", convertStr) == 1) {
                            io_data_object->fParameterIndexNr = (guint32)strtoul(convertStr, NULL, 0);
                        }
                        break;    /
                    }
                }

                memset (temp, 0, sizeof(*temp));
                fseek(fp, 0, SEEK_SET);                /

                while(pn_fgets(temp, MAX_LINE_LENGTH, fp) != NULL) {
                    if((strstr(temp, moduleStr)) != NULL) {                         /
                        memset (convertStr, 0, sizeof(*convertStr));
                        pch = strstr(temp, moduleStr);                              /
                        if (pch != NULL && sscanf(pch, "ModuleIdentNumber=\"%[^\"]", convertStr) == 1) {  /
                            read_module_id = (guint32)strtoul(convertStr, NULL, 0);     /

                            /
                            if (read_module_id == io_data_object->moduleIdentNr) {
                                ++io_data_object->amountInGSDML;    /

                                while(pn_fgets(temp, MAX_LINE_LENGTH, fp) != NULL) {
                                    if((strstr(temp, moduleNameInfo)) != NULL) {                    /
                                        long filePosRecord;

                                        if (sscanf(temp, "%*s TextId=\"%[^\"]", tmp_moduletext) != 1)        /
                                            break;

                                        filePosRecord = ftell(fp);            /
                                        /
                                        if (filePosRecord >= 0) {
                                            while (pn_fgets(temp, MAX_LINE_LENGTH, fp) != NULL && io_data_object->amountInGSDML == 1) {
                                                /
                                                if(((strstr(temp, tmp_moduletext)) != NULL) && ((strstr(temp, moduleValueInfo)) != NULL)) {
                                                    pch = strstr(temp, moduleValueInfo);
                                                    if (pch != NULL && sscanf(pch, "Value=\"%[^\"]", io_data_object->moduleNameStr) == 1)
                                                        break;    /
                                                }
                                            }

                                            fseek(fp, filePosRecord, SEEK_SET);    /
                                        }
                                    }

                                    /
                                    if((strstr(temp, subModuleStr)) != NULL) {
                                        memset (convertStr, 0, sizeof(*convertStr));
                                        pch = strstr(temp, subModuleStr);
                                        if (pch != NULL && sscanf(pch, "SubmoduleIdentNumber=\"%[^\"]", convertStr) == 1) {
                                            read_submodule_id = (guint32) strtoul (convertStr, NULL, 0);    /

                                            /
                                            if(read_submodule_id == io_data_object->subModuleIdentNr) {
                                                if((strstr(temp, profisafeStr)) != NULL) {
                                                    io_data_object->profisafeSupported = TRUE;   /
                                                    break;
                                                }
                                                else {    /
                                                    while(pn_fgets(temp, MAX_LINE_LENGTH, fp) != NULL) {
                                                        if((strstr(temp, profisafeStr)) != NULL) {
                                                            io_data_object->profisafeSupported = TRUE;
                                                            break;    /
                                                        }

                                                        else if((strstr(temp, ">")) != NULL) {
                                                            break;
                                                        }
                                                    }
                                                }
                                            }
                                            break;    /
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                fclose(fp);
                fp = NULL;
            }

            switch (u16SubmoduleProperties & 0x03) {
            case(0x00): /
                offset = dissect_DataDescription(tvb, offset, pinfo, sub_tree, drep, io_data_object);
                break;
            case(0x01): /
                offset = dissect_DataDescription(tvb, offset, pinfo, sub_tree, drep, io_data_object);
                break;
            case(0x02): /
                offset = dissect_DataDescription(tvb, offset, pinfo, sub_tree, drep, io_data_object);
                break;
            case(0x03): /
                offset = dissect_DataDescription(tvb, offset, pinfo, sub_tree, drep, io_data_object);
                offset = dissect_DataDescription(tvb, offset, pinfo, sub_tree, drep, io_data_object);
                break;
            default: /
                break;
            }

            proto_item_append_text(sub_item, ": Subslot:0x%x, Ident:0x%x Properties:0x%x",
                u16SubslotNr, u32SubmoduleIdentNumber, u16SubmoduleProperties);
            proto_item_set_len(sub_item, offset - u32SubStart);
        }

        proto_item_set_len(api_item, offset - u32ApiStart);
    }

    return offset;
}