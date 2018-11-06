static uint32_t nvic_readl(NVICState *s, uint32_t offset, MemTxAttrs attrs)
{
    ARMCPU *cpu = s->cpu;
    uint32_t val;

    switch (offset) {
    case 4: /
        return ((s->num_irq - NVIC_FIRST_IRQ) / 32) - 1;
    case 0x380 ... 0x3bf: /
    {
        int startvec = 32 * (offset - 0x380) + NVIC_FIRST_IRQ;
        int i;

        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (!attrs.secure) {
            return 0;
        }
        val = 0;
        for (i = 0; i < 32 && startvec + i < s->num_irq; i++) {
            if (s->itns[startvec + i]) {
                val |= (1 << i);
            }
        }
        return val;
    }
    case 0xd00: /
        return cpu->midr;
    case 0xd04: /
        /
        val = cpu->env.v7m.exception;
        /
        val |= (s->vectpending & 0xff) << 12;
        /
        if (nvic_isrpending(s)) {
            val |= (1 << 22);
        }
        /
        if (nvic_rettobase(s)) {
            val |= (1 << 11);
        }
        if (attrs.secure) {
            /
            if (s->sec_vectors[ARMV7M_EXCP_SYSTICK].pending) {
                val |= (1 << 26);
            }
            /
            if (s->sec_vectors[ARMV7M_EXCP_PENDSV].pending) {
                val |= (1 << 28);
            }
        } else {
            /
            if (s->vectors[ARMV7M_EXCP_SYSTICK].pending) {
                val |= (1 << 26);
            }
            /
            if (s->vectors[ARMV7M_EXCP_PENDSV].pending) {
                val |= (1 << 28);
            }
        }
        /
        if ((cpu->env.v7m.aircr & R_V7M_AIRCR_BFHFNMINS_MASK) &&
            s->vectors[ARMV7M_EXCP_NMI].pending) {
            val |= (1 << 31);
        }
        /
        /
        return val;
    case 0xd08: /
        return cpu->env.v7m.vecbase[attrs.secure];
    case 0xd0c: /
        val = 0xfa050000 | (s->prigroup[attrs.secure] << 8);
        if (attrs.secure) {
            /
            val |= cpu->env.v7m.aircr;
        } else {
            if (arm_feature(&cpu->env, ARM_FEATURE_V8)) {
                /
                val |= cpu->env.v7m.aircr & R_V7M_AIRCR_BFHFNMINS_MASK;
            }
        }
        return val;
    case 0xd10: /
        /
        return 0;
    case 0xd14: /
        /
        val = cpu->env.v7m.ccr[attrs.secure];
        val |= cpu->env.v7m.ccr[M_REG_NS] & R_V7M_CCR_BFHFNMIGN_MASK;
        return val;
    case 0xd24: /
        val = 0;
        if (attrs.secure) {
            if (s->sec_vectors[ARMV7M_EXCP_MEM].active) {
                val |= (1 << 0);
            }
            if (s->sec_vectors[ARMV7M_EXCP_HARD].active) {
                val |= (1 << 2);
            }
            if (s->sec_vectors[ARMV7M_EXCP_USAGE].active) {
                val |= (1 << 3);
            }
            if (s->sec_vectors[ARMV7M_EXCP_SVC].active) {
                val |= (1 << 7);
            }
            if (s->sec_vectors[ARMV7M_EXCP_PENDSV].active) {
                val |= (1 << 10);
            }
            if (s->sec_vectors[ARMV7M_EXCP_SYSTICK].active) {
                val |= (1 << 11);
            }
            if (s->sec_vectors[ARMV7M_EXCP_USAGE].pending) {
                val |= (1 << 12);
            }
            if (s->sec_vectors[ARMV7M_EXCP_MEM].pending) {
                val |= (1 << 13);
            }
            if (s->sec_vectors[ARMV7M_EXCP_SVC].pending) {
                val |= (1 << 15);
            }
            if (s->sec_vectors[ARMV7M_EXCP_MEM].enabled) {
                val |= (1 << 16);
            }
            if (s->sec_vectors[ARMV7M_EXCP_USAGE].enabled) {
                val |= (1 << 18);
            }
            if (s->sec_vectors[ARMV7M_EXCP_HARD].pending) {
                val |= (1 << 21);
            }
            /
            if (s->vectors[ARMV7M_EXCP_SECURE].active) {
                val |= (1 << 4);
            }
            if (s->vectors[ARMV7M_EXCP_SECURE].enabled) {
                val |= (1 << 19);
            }
            if (s->vectors[ARMV7M_EXCP_SECURE].pending) {
                val |= (1 << 20);
            }
        } else {
            if (s->vectors[ARMV7M_EXCP_MEM].active) {
                val |= (1 << 0);
            }
            if (arm_feature(&cpu->env, ARM_FEATURE_V8)) {
                /
                if (s->vectors[ARMV7M_EXCP_HARD].active) {
                    val |= (1 << 2);
                }
                if (s->vectors[ARMV7M_EXCP_HARD].pending) {
                    val |= (1 << 21);
                }
            }
            if (s->vectors[ARMV7M_EXCP_USAGE].active) {
                val |= (1 << 3);
            }
            if (s->vectors[ARMV7M_EXCP_SVC].active) {
                val |= (1 << 7);
            }
            if (s->vectors[ARMV7M_EXCP_PENDSV].active) {
                val |= (1 << 10);
            }
            if (s->vectors[ARMV7M_EXCP_SYSTICK].active) {
                val |= (1 << 11);
            }
            if (s->vectors[ARMV7M_EXCP_USAGE].pending) {
                val |= (1 << 12);
            }
            if (s->vectors[ARMV7M_EXCP_MEM].pending) {
                val |= (1 << 13);
            }
            if (s->vectors[ARMV7M_EXCP_SVC].pending) {
                val |= (1 << 15);
            }
            if (s->vectors[ARMV7M_EXCP_MEM].enabled) {
                val |= (1 << 16);
            }
            if (s->vectors[ARMV7M_EXCP_USAGE].enabled) {
                val |= (1 << 18);
            }
        }
        if (attrs.secure || (cpu->env.v7m.aircr & R_V7M_AIRCR_BFHFNMINS_MASK)) {
            if (s->vectors[ARMV7M_EXCP_BUS].active) {
                val |= (1 << 1);
            }
            if (s->vectors[ARMV7M_EXCP_BUS].pending) {
                val |= (1 << 14);
            }
            if (s->vectors[ARMV7M_EXCP_BUS].enabled) {
                val |= (1 << 17);
            }
            if (arm_feature(&cpu->env, ARM_FEATURE_V8) &&
                s->vectors[ARMV7M_EXCP_NMI].active) {
                /
                val |= (1 << 5);
            }
        }

        /
        if (s->vectors[ARMV7M_EXCP_DEBUG].active) {
            val |= (1 << 8);
        }
        return val;
    case 0xd28: /
        /
        val = cpu->env.v7m.cfsr[attrs.secure];
        val |= cpu->env.v7m.cfsr[M_REG_NS] & R_V7M_CFSR_BFSR_MASK;
        return val;
    case 0xd2c: /
        return cpu->env.v7m.hfsr;
    case 0xd30: /
        return cpu->env.v7m.dfsr;
    case 0xd34: /
        return cpu->env.v7m.mmfar[attrs.secure];
    case 0xd38: /
        return cpu->env.v7m.bfar;
    case 0xd3c: /
        /
        qemu_log_mask(LOG_UNIMP,
                      "Aux Fault status registers unimplemented\n");
        return 0;
    case 0xd40: /
        return 0x00000030;
    case 0xd44: /
        return 0x00000200;
    case 0xd48: /
        return 0x00100000;
    case 0xd4c: /
        return 0x00000000;
    case 0xd50: /
        return 0x00000030;
    case 0xd54: /
        return 0x00000000;
    case 0xd58: /
        return 0x00000000;
    case 0xd5c: /
        return 0x00000000;
    case 0xd60: /
        return 0x01141110;
    case 0xd64: /
        return 0x02111000;
    case 0xd68: /
        return 0x21112231;
    case 0xd6c: /
        return 0x01111110;
    case 0xd70: /
        return 0x01310102;
    /
    case 0xd90: /
        /
        return cpu->pmsav7_dregion << 8;
        break;
    case 0xd94: /
        return cpu->env.v7m.mpu_ctrl[attrs.secure];
    case 0xd98: /
        return cpu->env.pmsav7.rnr[attrs.secure];
    case 0xd9c: /
    case 0xda4: /
    case 0xdac: /
    case 0xdb4: /
    {
        int region = cpu->env.pmsav7.rnr[attrs.secure];

        if (arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            /
            int aliasno = (offset - 0xd9c) / 8; /
            if (aliasno) {
                region = deposit32(region, 0, 2, aliasno);
            }
            if (region >= cpu->pmsav7_dregion) {
                return 0;
            }
            return cpu->env.pmsav8.rbar[attrs.secure][region];
        }

        if (region >= cpu->pmsav7_dregion) {
            return 0;
        }
        return (cpu->env.pmsav7.drbar[region] & 0x1f) | (region & 0xf);
    }
    case 0xda0: /
    case 0xda8: /
    case 0xdb0: /
    case 0xdb8: /
    {
        int region = cpu->env.pmsav7.rnr[attrs.secure];

        if (arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            /
            int aliasno = (offset - 0xda0) / 8; /
            if (aliasno) {
                region = deposit32(region, 0, 2, aliasno);
            }
            if (region >= cpu->pmsav7_dregion) {
                return 0;
            }
            return cpu->env.pmsav8.rlar[attrs.secure][region];
        }

        if (region >= cpu->pmsav7_dregion) {
            return 0;
        }
        return ((cpu->env.pmsav7.dracr[region] & 0xffff) << 16) |
            (cpu->env.pmsav7.drsr[region] & 0xffff);
    }
    case 0xdc0: /
        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        return cpu->env.pmsav8.mair0[attrs.secure];
    case 0xdc4: /
        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        return cpu->env.pmsav8.mair1[attrs.secure];
    case 0xdd0: /
        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (!attrs.secure) {
            return 0;
        }
        return cpu->env.sau.ctrl;
    case 0xdd4: /
        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (!attrs.secure) {
            return 0;
        }
        return cpu->sau_sregion;
    case 0xdd8: /
        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (!attrs.secure) {
            return 0;
        }
        return cpu->env.sau.rnr;
    case 0xddc: /
    {
        int region = cpu->env.sau.rnr;

        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (!attrs.secure) {
            return 0;
        }
        if (region >= cpu->sau_sregion) {
            return 0;
        }
        return cpu->env.sau.rbar[region];
    }
    case 0xde0: /
    {
        int region = cpu->env.sau.rnr;

        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (!attrs.secure) {
            return 0;
        }
        if (region >= cpu->sau_sregion) {
            return 0;
        }
        return cpu->env.sau.rlar[region];
    }
    case 0xde4: /
        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (!attrs.secure) {
            return 0;
        }
        return cpu->env.v7m.sfsr;
    case 0xde8: /
        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (!attrs.secure) {
            return 0;
        }
        return cpu->env.v7m.sfar;
    default:
    bad_offset:
        qemu_log_mask(LOG_GUEST_ERROR, "NVIC: Bad read offset 0x%x\n", offset);
        return 0;
    }
}
