static CPUArchState *find_cpu(uint32_t thread_id)
{
    CPUState *cpu;

    cpu = qemu_get_cpu(thread_id);
    if (cpu == NULL) {
        return NULL;
    }
    return cpu->env_ptr;
}
