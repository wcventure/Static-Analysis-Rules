static int
dissect_ndr_ucarray_core(tvbuff_t *tvb, gint offset, packet_info *pinfo,
                    proto_tree *tree, dcerpc_info *di, guint8 *drep,
                    dcerpc_dissect_fnct_t *fnct_bytes,
                    dcerpc_dissect_fnct_blk_t *fnct_block)
{
    guint32      i;
    int          old_offset;
    int          conformance_size = 4;

    /
    DISSECTOR_ASSERT((fnct_bytes && !fnct_block) || (!fnct_bytes && fnct_block));

    if (di->call_data->flags & DCERPC_IS_NDR64) {
        conformance_size = 8;
    }

    if (di->conformant_run) {
        guint64 val;

        /
        old_offset = offset;
        di->conformant_run = 0;
        offset = dissect_ndr_uint3264(tvb, offset, pinfo, tree, di, drep,
                                       hf_dcerpc_array_max_count, &val);
        di->array_max_count = (gint32)val;
        di->array_max_count_offset = offset-conformance_size;
        di->conformant_run = 1;
        di->conformant_eaten = offset-old_offset;
    } else {
        /
        proto_tree_add_uint(tree, hf_dcerpc_array_max_count, tvb, di->array_max_count_offset, conformance_size, di->array_max_count);

        /
        if (fnct_block) {
                old_offset = offset;
                offset = (*fnct_block)(tvb, offset, di->array_max_count,
                                       pinfo, tree, di, drep);
                if (offset <= old_offset)
                    THROW(ReportedBoundsError);
        } else {
            for (i=0 ;i<di->array_max_count; i++) {
                old_offset = offset;
                offset = (*fnct_bytes)(tvb, offset, pinfo, tree, di, drep);
                if (offset <= old_offset)
                    THROW(ReportedBoundsError);
            }
        }
    }

    return offset;
}
