static void
mcaststream_draw(void *ti_ptr)
{
    mcaststream_tapinfo_t *tapinfo = (mcaststream_tapinfo_t *)ti_ptr;
/
    if (tapinfo && tapinfo->tap_draw) {
        tapinfo->tap_draw(ti_ptr);
    }
    return;
}
