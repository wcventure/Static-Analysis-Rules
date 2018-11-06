static int vmstate_subsection_load(QEMUFile *f, const VMStateDescription *vmsd,
                                   void *opaque)
{
    const VMStateSubsection *sub = vmsd->subsections;

    if (!sub || !sub->needed) {
        return 0;
    }

    while (qemu_peek_byte(f) == QEMU_VM_SUBSECTION) {
        char idstr[256];
        int ret;
        uint8_t version_id, len;
        const VMStateDescription *sub_vmsd;

        qemu_get_byte(f); /
        len = qemu_get_byte(f);
        qemu_get_buffer(f, (uint8_t *)idstr, len);
        idstr[len] = 0;
        version_id = qemu_get_be32(f);

        sub_vmsd = vmstate_get_subsection(sub, idstr);
        if (sub_vmsd == NULL) {
            return -ENOENT;
        }
        assert(!sub_vmsd->subsections);
        ret = vmstate_load_state(f, sub_vmsd, opaque, version_id);
        if (ret) {
            return ret;
        }
    }
    return 0;
}
