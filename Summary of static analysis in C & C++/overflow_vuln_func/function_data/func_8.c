static void
mcaststream_reset_cb(void *ti_ptr)
{
    mcaststream_tapinfo_t *tapinfo = (mcaststream_tapinfo_t *)ti_ptr;
    if (tapinfo) {
        if (tapinfo->tap_reset) {
           tapinfo->tap_reset(ti_ptr);
        }
        mcaststream_reset(tapinfo);
    }
}
