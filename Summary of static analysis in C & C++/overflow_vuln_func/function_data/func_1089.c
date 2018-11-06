static gboolean pcapng_dump_close(wtap_dumper *wdh, int *err _U_)
{
    guint i, j;

    /
    pcapng_write_name_resolution_block(wdh, err);

    for (i = 0; i < wdh->interface_data->len; i++) {

        /
        wtapng_if_descr_t int_data;

        int_data = g_array_index(wdh->interface_data, wtapng_if_descr_t, i);
        for (j = 0; j < int_data.num_stat_entries; j++) {
            wtapng_if_stats_t if_stats;

            if_stats = g_array_index(int_data.interface_statistics, wtapng_if_stats_t, j);
            pcapng_debug1("pcapng_dump_close: write ISB for interface %u",if_stats.interface_id);
            if (!pcapng_write_interface_statistics_block(wdh, &if_stats, err)) {
                return FALSE;
            }
        }
    }

    pcapng_debug0("pcapng_dump_close");
    return TRUE;
}
