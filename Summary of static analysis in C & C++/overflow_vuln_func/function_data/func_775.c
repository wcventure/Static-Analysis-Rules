static int
sam_dissect_SAM_SECURITY_DESCRIPTOR_data(tvbuff_t *tvb, int offset,
                             packet_info *pinfo, proto_tree *tree,
                             guint8 *drep)
{
       guint32 len;
       dcerpc_info *di;
       int old_offset = offset;

       di=pinfo->private_data;
       if(di->conformant_run){
               /
               return offset;
       }

       offset = dissect_ndr_uint32 (tvb, offset, pinfo, tree, drep,
                                    hf_samr_sd_size, &len);

       dissect_nt_sec_desc(
               tvb, offset, pinfo, tree, drep, len, &samr_connect_access_mask_info);

       offset += len;
       if (offset < old_offset)
               THROW(ReportedBoundsError);

       return offset;
}
