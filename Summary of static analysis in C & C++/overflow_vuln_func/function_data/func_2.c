static guint32 DOFObjectID_GetClassSize_Bytes(const guint8 *pBytes)
{
    guint32 size = 4;

    (void)OALMarshal_UncompressValue(DOFOBJECTID_MAX_CLASS_SIZE, &size, pBytes);

    return size;
}
