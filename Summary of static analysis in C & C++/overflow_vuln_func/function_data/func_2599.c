static int vdi_check(BlockDriverState *bs, BdrvCheckResult *res,
                     BdrvCheckMode fix)
{
    /
    BDRVVdiState *s = (BDRVVdiState *)bs->opaque;
    uint32_t blocks_allocated = 0;
    uint32_t block;
    uint32_t *bmap;
    logout("\n");

    if (fix) {
        return -ENOTSUP;
    }

    bmap = g_try_malloc(s->header.blocks_in_image * sizeof(uint32_t));
    if (s->header.blocks_in_image && bmap == NULL) {
        res->check_errors++;
        return -ENOMEM;
    }

    memset(bmap, 0xff, s->header.blocks_in_image * sizeof(uint32_t));

    /
    for (block = 0; block < s->header.blocks_in_image; block++) {
        uint32_t bmap_entry = le32_to_cpu(s->bmap[block]);
        if (VDI_IS_ALLOCATED(bmap_entry)) {
            if (bmap_entry < s->header.blocks_in_image) {
                blocks_allocated++;
                if (!VDI_IS_ALLOCATED(bmap[bmap_entry])) {
                    bmap[bmap_entry] = bmap_entry;
                } else {
                    fprintf(stderr, "ERROR: block index %" PRIu32
                            " also used by %" PRIu32 "\n", bmap[bmap_entry], bmap_entry);
                    res->corruptions++;
                }
            } else {
                fprintf(stderr, "ERROR: block index %" PRIu32
                        " too large, is %" PRIu32 "\n", block, bmap_entry);
                res->corruptions++;
            }
        }
    }
    if (blocks_allocated != s->header.blocks_allocated) {
        fprintf(stderr, "ERROR: allocated blocks mismatch, is %" PRIu32
               ", should be %" PRIu32 "\n",
               blocks_allocated, s->header.blocks_allocated);
        res->corruptions++;
    }

    g_free(bmap);

    return 0;
}
