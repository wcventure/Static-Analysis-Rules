long WINAPI
libAVMemInputPin_QueryInterface(libAVMemInputPin *this, const GUID *riid,
                                void **ppvObject)
{
    libAVPin *pin = (libAVPin *) ((uint8_t *) this - imemoffset);
    dshowdebug("libAVMemInputPin_QueryInterface(%p)\n", this);
    return libAVPin_QueryInterface(pin, riid, ppvObject);
}
unsigned long WINAPI
libAVMemInputPin_AddRef(libAVMemInputPin *this)
{
    libAVPin *pin = (libAVPin *) ((uint8_t *) this - imemoffset);
    dshowdebug("libAVMemInputPin_AddRef(%p)\n", this);
    return libAVPin_AddRef(pin);
}
unsigned long WINAPI
libAVMemInputPin_Release(libAVMemInputPin *this)
{
    libAVPin *pin = (libAVPin *) ((uint8_t *) this - imemoffset);
    dshowdebug("libAVMemInputPin_Release(%p)\n", this);
    return libAVPin_Release(pin);
}
long WINAPI
libAVMemInputPin_GetAllocator(libAVMemInputPin *this, IMemAllocator **alloc)
{
    dshowdebug("libAVMemInputPin_GetAllocator(%p)\n", this);
    return VFW_E_NO_ALLOCATOR;
}
long WINAPI
libAVMemInputPin_NotifyAllocator(libAVMemInputPin *this, IMemAllocator *alloc,
                                 BOOL rdwr)
{
    dshowdebug("libAVMemInputPin_NotifyAllocator(%p)\n", this);
    return S_OK;
}
long WINAPI
libAVMemInputPin_GetAllocatorRequirements(libAVMemInputPin *this,
                                          ALLOCATOR_PROPERTIES *props)
{
    dshowdebug("libAVMemInputPin_GetAllocatorRequirements(%p)\n", this);
    return E_NOTIMPL;
}
long WINAPI
libAVMemInputPin_Receive(libAVMemInputPin *this, IMediaSample *sample)
{
    libAVPin *pin = (libAVPin *) ((uint8_t *) this - imemoffset);
    enum dshowDeviceType devtype = pin->filter->type;
    void *priv_data;
    uint8_t *buf;
    int buf_size;
    int index;
    int64_t curtime;

    dshowdebug("libAVMemInputPin_Receive(%p)\n", this);

    if (!sample)
        return E_POINTER;

    if (devtype == VideoDevice) {
        /
        IReferenceClock *clock = pin->filter->clock;
        IReferenceClock_GetTime(clock, &curtime);
    } else {
        int64_t dummy;
        IMediaSample_GetTime(sample, &curtime, &dummy);
        curtime += pin->filter->start_time;
    }

    buf_size = IMediaSample_GetActualDataLength(sample);
    IMediaSample_GetPointer(sample, &buf);
    priv_data = pin->filter->priv_data;
    index = pin->filter->stream_index;

    pin->filter->callback(priv_data, index, buf, buf_size, curtime);

    return S_OK;
}
