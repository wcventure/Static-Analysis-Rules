static const uint8_t *avc_mp4_find_startcode(const uint8_t *start, const uint8_t *end, int nal_length_size)
{
    int res = 0;

    if (end - start < nal_length_size)
        return NULL;
    while (nal_length_size--)
        res = (res << 8) | *start++;

    if (start + res > end || res < 0 || start + res < start)
        return NULL;

    return start + res;
}
