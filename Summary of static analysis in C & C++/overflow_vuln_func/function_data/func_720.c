static void dissect_packet_all_in(proto_tree *gvsp_tree, tvbuff_t *tvb, gint offset, packet_info *pinfo, gvsp_packet_info *info)
{
    switch (info->payloadtype)
    {
    case GVSP_PAYLOAD_IMAGE:
        offset += dissect_image_leader(gvsp_tree, tvb, pinfo, offset);
        offset += dissect_image_trailer(gvsp_tree, tvb, pinfo, offset);
        if (info->chunk != 0)
        {
            offset += dissect_extra_chunk_info(gvsp_tree, tvb, pinfo, offset);
        }
        dissect_packet_payload(gvsp_tree, tvb, pinfo, offset);
        break;

    case GVSP_PAYLOAD_RAWDATA:
        offset += dissect_raw_data_leader(gvsp_tree, tvb, pinfo, offset);
        offset += dissect_generic_trailer(gvsp_tree, tvb, pinfo, offset);
        if (info->chunk != 0)
        {
            offset += dissect_extra_chunk_info(gvsp_tree, tvb, pinfo, offset);
        }
        dissect_packet_payload(gvsp_tree, tvb, pinfo, offset);
        break;

    case GVSP_PAYLOAD_FILE:
        offset += dissect_file_leader(gvsp_tree, tvb, pinfo, offset);
        offset += dissect_generic_trailer(gvsp_tree, tvb, pinfo, offset);
        if (info->chunk != 0)
        {
            offset += dissect_extra_chunk_info(gvsp_tree, tvb, pinfo, offset);
        }
        dissect_packet_payload(gvsp_tree, tvb, pinfo, offset);
        break;

    case GVSP_PAYLOAD_CHUNKDATA:
        offset += dissect_chunk_data_leader(gvsp_tree, tvb, pinfo, offset);
        offset += dissect_chunk_data_trailer(gvsp_tree, tvb, pinfo, offset);
        if (info->chunk != 0)
        {
            offset += dissect_extra_chunk_info(gvsp_tree, tvb, pinfo, offset);
        }
        dissect_packet_payload(gvsp_tree, tvb, pinfo, offset);
        break;

    case GVSP_PAYLOAD_EXTENDEDCHUNKDATA:
        offset += dissect_extended_chunk_data_leader(gvsp_tree, tvb, pinfo, offset);
        offset += dissect_extended_chunk_data_trailer(gvsp_tree, tvb, pinfo, offset);
        if (info->chunk != 0)
        {
            offset += dissect_extra_chunk_info(gvsp_tree, tvb, pinfo, offset);
        }
        dissect_packet_payload(gvsp_tree, tvb, pinfo, offset);
        break;

    case GVSP_PAYLOAD_JPEG:
    case GVSP_PAYLOAD_JPEG2000:
        offset += dissect_jpeg_leader(gvsp_tree, tvb, pinfo, offset);
        offset += dissect_generic_trailer(gvsp_tree, tvb, pinfo, offset);
        if (info->chunk != 0)
        {
            offset += dissect_extra_chunk_info(gvsp_tree, tvb, pinfo, offset);
        }
        dissect_packet_payload(gvsp_tree, tvb, pinfo, offset);
        break;

    case GVSP_PAYLOAD_H264:
        offset += dissect_h264_leader(gvsp_tree, tvb, pinfo, offset);
        offset += dissect_generic_trailer(gvsp_tree, tvb, pinfo, offset);
        if (info->chunk != 0)
        {
            offset += dissect_extra_chunk_info(gvsp_tree, tvb, pinfo, offset);
        }
        dissect_packet_payload_h264(gvsp_tree, tvb, pinfo, offset);
        break;

    case GVSP_PAYLOAD_MULTIZONEIMAGE:
        offset += dissect_multizone_image_leader(gvsp_tree, tvb, pinfo, offset);
        offset += dissect_image_trailer(gvsp_tree, tvb, pinfo, offset);
        if (info->chunk != 0)
        {
            offset += dissect_extra_chunk_info(gvsp_tree, tvb, pinfo, offset);
        }
        dissect_packet_payload_multizone(gvsp_tree, tvb, pinfo, offset);
        break;
    }
}
