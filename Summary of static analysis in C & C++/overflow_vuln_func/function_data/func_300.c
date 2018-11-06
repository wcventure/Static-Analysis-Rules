static int
dissect_ndr_ucvarray_core(tvbuff_t *tvb, gint offset, packet_info *pinfo,
                     proto_tree *tree, dcerpc_info *di, guint8 *drep,
                     dcerpc_dissect_fnct_t *fnct_bytes,
                     dcerpc_dissect_fnct_blk_t *fnct_block)
{
    guint32      i;
    int          old_offset;
    int          conformance_size = 4;

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
        DISSECTOR_ASSERT(val <= G_MAXUINT32);
        di->array_max_count = (guint32)val;
        di->array_max_count_offset = offset-conformance_size;
        offset = dissect_ndr_uint3264(tvb, offset, pinfo, tree, di, drep,
                                       hf_dcerpc_array_offset, &val);
        DISSECTOR_ASSERT(val <= G_MAXUINT32);
        di->array_offset = (guint32)val;
        di->array_offset_offset = offset-conformance_size;
        offset = dissect_ndr_uint3264(tvb, offset, pinfo, tree, di, drep,
                                       hf_dcerpc_array_actual_count, &val);
        DISSECTOR_ASSERT(val <= G_MAXUINT32);
        di->array_actual_count = (guint32)val;
        di->array_actual_count_offset = offset-conformance_size;
        di->conformant_run = 1;
        di->conformant_eaten = offset-old_offset;
    } else {
        /
        proto_tree_add_uint(tree, hf_dcerpc_array_max_count, tvb, di->array_max_count_offset, conformance_size, di->array_max_count);
        proto_tree_add_uint(tree, hf_dcerpc_array_offset, tvb, di->array_offset_offset, conformance_size, di->array_offset);
        proto_tree_add_uint(tree, hf_dcerpc_array_actual_count, tvb, di->array_actual_count_offset, conformance_size, di->array_actual_count);

        /
        if (fnct_block) {
                old_offset = offset;
                offset = (*fnct_block)(tvb, offset, di->array_actual_count,
                                       pinfo, tree, di, drep);
                if (offset <= old_offset)
                    THROW(ReportedBoundsError);
        } else if (fnct_bytes) {
            for (i=0 ;i<di->array_actual_count; i++) {
                old_offset = offset;
                offset = (*fnct_bytes)(tvb, offset, pinfo, tree, di, drep);
                if (offset <= old_offset)
                    THROW(ReportedBoundsError);
            }
        }
    }

    return offset;
}
