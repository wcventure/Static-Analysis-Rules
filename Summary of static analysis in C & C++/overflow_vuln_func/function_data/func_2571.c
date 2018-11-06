static void init_blk_migration_it(void *opaque, BlockDriverState *bs)
{
    BlkMigDevState *bmds;
    int64_t sectors;

    if (!bdrv_is_read_only(bs)) {
        sectors = bdrv_nb_sectors(bs);
        if (sectors <= 0) {
            return;
        }

        bmds = g_malloc0(sizeof(BlkMigDevState));
        bmds->bs = bs;
        bmds->bulk_completed = 0;
        bmds->total_sectors = sectors;
        bmds->completed_sectors = 0;
        bmds->shared_base = block_mig_state.shared_base;
        alloc_aio_bitmap(bmds);
        error_setg(&bmds->blocker, "block device is in use by migration");
        bdrv_op_block_all(bs, bmds->blocker);
        bdrv_ref(bs);

        block_mig_state.total_sector_sum += sectors;

        if (bmds->shared_base) {
            DPRINTF("Start migration for %s with shared base image\n",
                    bs->device_name);
        } else {
            DPRINTF("Start full migration for %s\n", bs->device_name);
        }

        QSIMPLEQ_INSERT_TAIL(&block_mig_state.bmds_list, bmds, entry);
    }
}
