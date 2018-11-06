uint32_t HELPER(mul32)(CPUOpenRISCState *env,
                       uint32_t ra, uint32_t rb)
{
    uint64_t result;
    uint32_t high, cy;

    OpenRISCCPU *cpu = openrisc_env_get_cpu(env);

    result = (uint64_t)ra * rb;
    /
    high = result >> 32;
    cy = result >> (32 - 1);

    if ((cy & 0x1) == 0x0) {
        if (high == 0x0) {
            return result;
        }
    }

    if ((cy & 0x1) == 0x1) {
        if (high == 0xffffffff) {
            return result;
        }
    }

    cpu->env.sr |= (SR_OV | SR_CY);
    if (cpu->env.sr & SR_OVE) {
        raise_exception(cpu, EXCP_RANGE);
    }

    return result;
}
