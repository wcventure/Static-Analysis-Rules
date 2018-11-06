int bdrv_drop_intermediate(BlockDriverState *active, BlockDriverState *top,
                           BlockDriverState *base, const char *backing_file_str)
{
    BlockDriverState *intermediate;
    BlockDriverState *base_bs = NULL;
    BlockDriverState *new_top_bs = NULL;
    BlkIntermediateStates *intermediate_state, *next;
    int ret = -EIO;

    QSIMPLEQ_HEAD(states_to_delete, BlkIntermediateStates) states_to_delete;
    QSIMPLEQ_INIT(&states_to_delete);

    if (!top->drv || !base->drv) {
        goto exit;
    }

    new_top_bs = bdrv_find_overlay(active, top);

    if (new_top_bs == NULL) {
        /
        goto exit;
    }

    /
    if (new_top_bs->backing_hd == base) {
        ret = 0;
        goto exit;
    }

    intermediate = top;

    /
    while (intermediate) {
        intermediate_state = g_malloc0(sizeof(BlkIntermediateStates));
        intermediate_state->bs = intermediate;
        QSIMPLEQ_INSERT_TAIL(&states_to_delete, intermediate_state, entry);

        if (intermediate->backing_hd == base) {
            base_bs = intermediate->backing_hd;
            break;
        }
        intermediate = intermediate->backing_hd;
    }
    if (base_bs == NULL) {
        /
        goto exit;
    }

    /
    backing_file_str = backing_file_str ? backing_file_str : base_bs->filename;
    ret = bdrv_change_backing_file(new_top_bs, backing_file_str,
                                   base_bs->drv ? base_bs->drv->format_name : "");
    if (ret) {
        goto exit;
    }
    bdrv_set_backing_hd(new_top_bs, base_bs);

    QSIMPLEQ_FOREACH_SAFE(intermediate_state, &states_to_delete, entry, next) {
        /
        bdrv_set_backing_hd(intermediate_state->bs, NULL);
        bdrv_unref(intermediate_state->bs);
    }
    ret = 0;

exit:
    QSIMPLEQ_FOREACH_SAFE(intermediate_state, &states_to_delete, entry, next) {
        g_free(intermediate_state);
    }
    return ret;
}
