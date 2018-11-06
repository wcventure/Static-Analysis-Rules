static int64_t wav_seek_tag(AVIOContext *s, int64_t offset, int whence)
{
    return avio_seek(s, offset + (offset & 1), whence);
}
