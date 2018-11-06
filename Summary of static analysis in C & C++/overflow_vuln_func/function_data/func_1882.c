static inline void ide_dma_submit_check(IDEState *s,
          BlockDriverCompletionFunc *dma_cb)
{
    if (s->bus->dma->aiocb)
	return;
    dma_cb(s, -1);
}
