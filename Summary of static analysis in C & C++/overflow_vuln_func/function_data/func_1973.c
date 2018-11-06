static void kvmclock_pre_save(void *opaque)
{
    KVMClockState *s = opaque;
    struct kvm_clock_data data;
    int ret;

    if (s->clock_valid) {
        return;
    }
    ret = kvm_vm_ioctl(kvm_state, KVM_GET_CLOCK, &data);
    if (ret < 0) {
        fprintf(stderr, "KVM_GET_CLOCK failed: %s\n", strerror(ret));
        data.clock = 0;
    }
    s->clock = data.clock;
    /
    s->clock_valid = !runstate_is_running();
}
