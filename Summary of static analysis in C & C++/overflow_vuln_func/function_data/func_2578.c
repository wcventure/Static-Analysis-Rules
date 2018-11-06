void bdrv_op_block(BlockDriverState *bs, BlockOpType op, Error *reason)
{
    BdrvOpBlocker *blocker;
    assert((int) op >= 0 && op < BLOCK_OP_TYPE_MAX);

    blocker = g_malloc0(sizeof(BdrvOpBlocker));
    blocker->reason = reason;
    QLIST_INSERT_HEAD(&bs->op_blockers[op], blocker, list);
}
