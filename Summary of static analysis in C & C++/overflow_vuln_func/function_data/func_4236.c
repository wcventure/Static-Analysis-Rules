static int mxf_read_generic_descriptor(void *arg, AVIOContext *pb, int tag, int size, UID uid)
{
    MXFDescriptor *descriptor = arg;
    switch(tag) {
    case 0x3F01:
        descriptor->sub_descriptors_count = avio_rb32(pb);
        if (descriptor->sub_descriptors_count >= UINT_MAX / sizeof(UID))
            return -1;
        descriptor->sub_descriptors_refs = av_malloc(descriptor->sub_descriptors_count * sizeof(UID));
        if (!descriptor->sub_descriptors_refs)
            return -1;
        avio_skip(pb, 4); /
        avio_read(pb, (uint8_t *)descriptor->sub_descriptors_refs, descriptor->sub_descriptors_count * sizeof(UID));
        break;
    case 0x3004:
        avio_read(pb, descriptor->essence_container_ul, 16);
        break;
    case 0x3006:
        descriptor->linked_track_id = avio_rb32(pb);
        break;
    case 0x3201: /
        avio_read(pb, descriptor->essence_codec_ul, 16);
        break;
    case 0x3203:
        descriptor->width = avio_rb32(pb);
        break;
    case 0x3202:
        descriptor->height = avio_rb32(pb);
        break;
    case 0x320E:
        descriptor->aspect_ratio.num = avio_rb32(pb);
        descriptor->aspect_ratio.den = avio_rb32(pb);
        break;
    case 0x3D03:
        descriptor->sample_rate.num = avio_rb32(pb);
        descriptor->sample_rate.den = avio_rb32(pb);
        break;
    case 0x3D06: /
        avio_read(pb, descriptor->essence_codec_ul, 16);
        break;
    case 0x3D07:
        descriptor->channels = avio_rb32(pb);
        break;
    case 0x3D01:
        descriptor->bits_per_sample = avio_rb32(pb);
        break;
    case 0x3401:
        mxf_read_pixel_layout(pb, descriptor);
        break;
    default:
        /
        if (IS_KLV_KEY(uid, mxf_sony_mpeg4_extradata)) {
            descriptor->extradata = av_malloc(size);
            if (!descriptor->extradata)
                return -1;
            descriptor->extradata_size = size;
            avio_read(pb, descriptor->extradata, size);
        }
        break;
    }
    return 0;
}
