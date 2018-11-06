static void avi_read_nikon(AVFormatContext *s, uint64_t end)
{
    while (avio_tell(s->pb) < end) {
        uint32_t tag  = avio_rl32(s->pb);
        uint32_t size = avio_rl32(s->pb);
        switch (tag) {
        case MKTAG('n', 'c', 't', 'g'):  /
        {
            uint64_t tag_end = avio_tell(s->pb) + size;
            while (avio_tell(s->pb) < tag_end) {
                uint16_t tag     = avio_rl16(s->pb);
                uint16_t size    = avio_rl16(s->pb);
                const char *name = NULL;
                char buffer[64]  = { 0 };
                if (avio_tell(s->pb) + size > tag_end)
                    size = tag_end - avio_tell(s->pb);
                size -= avio_read(s->pb, buffer,
                                  FFMIN(size, sizeof(buffer) - 1));
                switch (tag) {
                case 0x03:
                    name = "maker";
                    break;
                case 0x04:
                    name = "model";
                    break;
                case 0x13:
                    name = "creation_time";
                    if (buffer[4] == ':' && buffer[7] == ':')
                        buffer[4] = buffer[7] = '-';
                    break;
                }
                if (name)
                    av_dict_set(&s->metadata, name, buffer, 0);
                avio_skip(s->pb, size);
            }
            break;
        }
        default:
            avio_skip(s->pb, size);
            break;
        }
    }
}
