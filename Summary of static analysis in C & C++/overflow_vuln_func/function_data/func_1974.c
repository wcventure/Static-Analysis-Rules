static int kvmclock_post_load(void *opaque, int version_id)
{
    KVMClockState *s = opaque;
    struct kvm_clock_data data;

    data.clock = s->clock;
    data.flags = 0;
    return kvm_vm_ioctl(kvm_state, KVM_SET_CLOCK, &data);
}
