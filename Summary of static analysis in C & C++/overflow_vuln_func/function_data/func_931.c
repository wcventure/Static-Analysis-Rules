void
proto_reg_handoff_meta(void)
{
#if 0   /
    dissector_handle_t meta_handle;

    meta_handle = find_dissector("meta");
    dissector_add_uint("wtap_encap", WTAP_ENCAP_META, meta_handle);
#endif
    data_handle = find_dissector("data");
    alcap_handle = find_dissector("alcap");
    atm_untrunc_handle = find_dissector("atm_untruncated");
    nbap_handle = find_dissector("nbap");
    sscf_nni_handle = find_dissector("sscf-nni");
    ethwithfcs_handle = find_dissector("eth_withfcs");
    ethwithoutfcs_handle = find_dissector("eth_withoutfcs");
    fphint_handle = find_dissector("fp_hint");
    mtp2_handle = find_dissector("mtp2");
}
