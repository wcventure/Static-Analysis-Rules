const char *get_register_name_32(unsigned int reg)
{
    if (reg > CPU_NB_REGS32) {
        return NULL;
    }
    return x86_reg_info_32[reg].name;
}
