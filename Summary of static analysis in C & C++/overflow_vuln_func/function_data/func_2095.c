static int bdrv_check_byte_request(BlockDriverState *bs, int64_t offset,
                                   size_t size)
{
    int64_t len;

    if (!bdrv_is_inserted(bs))
        return -ENOMEDIUM;

    if (bs->growable)
        return 0;

    len = bdrv_getlength(bs);

    if ((offset + size) > len)
        return -EIO;

    return 0;
}
