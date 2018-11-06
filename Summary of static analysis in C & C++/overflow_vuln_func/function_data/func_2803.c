static void kvm_s390_stattrib_synchronize(S390StAttribState *sa)
{
    KVMS390StAttribState *sas = KVM_S390_STATTRIB(sa);
    MachineState *machine = MACHINE(qdev_get_machine());
    unsigned long max = machine->maxram_size / TARGET_PAGE_SIZE;
    unsigned long cx, len = 1 << 19;
    int r;
    struct kvm_s390_cmma_log clog = {
        .flags = 0,
        .mask = ~0ULL,
    };

    if (sas->incoming_buffer) {
        for (cx = 0; cx + len <= max; cx += len) {
            clog.start_gfn = cx;
            clog.count = len;
            clog.values = (uint64_t)(sas->incoming_buffer + cx * len);
            r = kvm_vm_ioctl(kvm_state, KVM_S390_SET_CMMA_BITS, &clog);
            if (r) {
                error_report("KVM_S390_SET_CMMA_BITS failed: %s", strerror(-r));
                return;
            }
        }
        if (cx < max) {
            clog.start_gfn = cx;
            clog.count = max - cx;
            clog.values = (uint64_t)(sas->incoming_buffer + cx * len);
            r = kvm_vm_ioctl(kvm_state, KVM_S390_SET_CMMA_BITS, &clog);
            if (r) {
                error_report("KVM_S390_SET_CMMA_BITS failed: %s", strerror(-r));
            }
        }
        g_free(sas->incoming_buffer);
        sas->incoming_buffer = NULL;
    }
}
