static void
dissect_fcp_data(tvbuff_t *tvb, packet_info *pinfo, proto_tree *parent_tree, conversation_t *conversation, fc_hdr *fchdr, itl_nexus_t *itl)
{
    dissect_scsi_payload(tvb, pinfo, parent_tree, FALSE, fchdr->itlq, itl);
}
