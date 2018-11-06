void
mcaststream_dlg_update(void *ti_ptr)
{
    GList *list;
    mcaststream_tapinfo_t *tapinfo = (mcaststream_tapinfo_t *)ti_ptr;

    if (!tapinfo) {
        return;
    }

    list = tapinfo->strinfo_list;

    if (mcast_stream_dlg != NULL) {
        gtk_list_store_clear(list_store);
        streams_nb = 0;

        list = g_list_first(list);
        while (list)
        {
            add_to_list_store((mcast_stream_info_t*)(list->data));
            list = g_list_next(list);
        }

        mcaststream_on_unselect(NULL, NULL);
    }

    last_list = list;
}
