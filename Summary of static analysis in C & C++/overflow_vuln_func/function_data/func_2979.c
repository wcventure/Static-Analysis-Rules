static void nvic_writel(NVICState *s, uint32_t offset, uint32_t value,
                        MemTxAttrs attrs)
{
    ARMCPU *cpu = s->cpu;

    switch (offset) {
    case 0x380 ... 0x3bf: /
    {
        int startvec = 32 * (offset - 0x380) + NVIC_FIRST_IRQ;
        int i;

        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (!attrs.secure) {
            break;
        }
        for (i = 0; i < 32 && startvec + i < s->num_irq; i++) {
            s->itns[startvec + i] = (value >> i) & 1;
        }
        nvic_irq_update(s);
        break;
    }
    case 0xd04: /
        if (cpu->env.v7m.aircr & R_V7M_AIRCR_BFHFNMINS_MASK) {
            if (value & (1 << 31)) {
                armv7m_nvic_set_pending(s, ARMV7M_EXCP_NMI, false);
            } else if (value & (1 << 30) &&
                       arm_feature(&cpu->env, ARM_FEATURE_V8)) {
                /
                armv7m_nvic_clear_pending(s, ARMV7M_EXCP_NMI, false);
            }
        }
        if (value & (1 << 28)) {
            armv7m_nvic_set_pending(s, ARMV7M_EXCP_PENDSV, attrs.secure);
        } else if (value & (1 << 27)) {
            armv7m_nvic_clear_pending(s, ARMV7M_EXCP_PENDSV, attrs.secure);
        }
        if (value & (1 << 26)) {
            armv7m_nvic_set_pending(s, ARMV7M_EXCP_SYSTICK, attrs.secure);
        } else if (value & (1 << 25)) {
            armv7m_nvic_clear_pending(s, ARMV7M_EXCP_SYSTICK, attrs.secure);
        }
        break;
    case 0xd08: /
        cpu->env.v7m.vecbase[attrs.secure] = value & 0xffffff80;
        break;
    case 0xd0c: /
        if ((value >> R_V7M_AIRCR_VECTKEY_SHIFT) == 0x05fa) {
            if (value & R_V7M_AIRCR_SYSRESETREQ_MASK) {
                if (attrs.secure ||
                    !(cpu->env.v7m.aircr & R_V7M_AIRCR_SYSRESETREQS_MASK)) {
                    qemu_irq_pulse(s->sysresetreq);
                }
            }
            if (value & R_V7M_AIRCR_VECTCLRACTIVE_MASK) {
                qemu_log_mask(LOG_GUEST_ERROR,
                              "Setting VECTCLRACTIVE when not in DEBUG mode "
                              "is UNPREDICTABLE\n");
            }
            if (value & R_V7M_AIRCR_VECTRESET_MASK) {
                /
                qemu_log_mask(LOG_GUEST_ERROR,
                              "Setting VECTRESET when not in DEBUG mode "
                              "is UNPREDICTABLE\n");
            }
            s->prigroup[attrs.secure] = extract32(value,
                                                  R_V7M_AIRCR_PRIGROUP_SHIFT,
                                                  R_V7M_AIRCR_PRIGROUP_LENGTH);
            if (attrs.secure) {
                /
                cpu->env.v7m.aircr = value &
                    (R_V7M_AIRCR_SYSRESETREQS_MASK |
                     R_V7M_AIRCR_BFHFNMINS_MASK |
                     R_V7M_AIRCR_PRIS_MASK);
                /
                if (cpu->env.v7m.aircr & R_V7M_AIRCR_BFHFNMINS_MASK) {
                    s->sec_vectors[ARMV7M_EXCP_HARD].prio = -3;
                    s->vectors[ARMV7M_EXCP_HARD].enabled = 1;
                } else {
                    s->sec_vectors[ARMV7M_EXCP_HARD].prio = -1;
                    s->vectors[ARMV7M_EXCP_HARD].enabled = 0;
                }
            }
            nvic_irq_update(s);
        }
        break;
    case 0xd10: /
        /
        qemu_log_mask(LOG_UNIMP, "NVIC: SCR unimplemented\n");
        break;
    case 0xd14: /
        /
        value &= (R_V7M_CCR_STKALIGN_MASK |
                  R_V7M_CCR_BFHFNMIGN_MASK |
                  R_V7M_CCR_DIV_0_TRP_MASK |
                  R_V7M_CCR_UNALIGN_TRP_MASK |
                  R_V7M_CCR_USERSETMPEND_MASK |
                  R_V7M_CCR_NONBASETHRDENA_MASK);

        if (arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            /
            value |= R_V7M_CCR_NONBASETHRDENA_MASK
                | R_V7M_CCR_STKALIGN_MASK;
        }
        if (attrs.secure) {
            /
            cpu->env.v7m.ccr[M_REG_NS] =
                (cpu->env.v7m.ccr[M_REG_NS] & ~R_V7M_CCR_BFHFNMIGN_MASK)
                | (value & R_V7M_CCR_BFHFNMIGN_MASK);
            value &= ~R_V7M_CCR_BFHFNMIGN_MASK;
        }

        cpu->env.v7m.ccr[attrs.secure] = value;
        break;
    case 0xd24: /
        if (attrs.secure) {
            s->sec_vectors[ARMV7M_EXCP_MEM].active = (value & (1 << 0)) != 0;
            /
            s->sec_vectors[ARMV7M_EXCP_USAGE].active = (value & (1 << 3)) != 0;
            s->sec_vectors[ARMV7M_EXCP_SVC].active = (value & (1 << 7)) != 0;
            s->sec_vectors[ARMV7M_EXCP_PENDSV].active =
                (value & (1 << 10)) != 0;
            s->sec_vectors[ARMV7M_EXCP_SYSTICK].active =
                (value & (1 << 11)) != 0;
            s->sec_vectors[ARMV7M_EXCP_USAGE].pending =
                (value & (1 << 12)) != 0;
            s->sec_vectors[ARMV7M_EXCP_MEM].pending = (value & (1 << 13)) != 0;
            s->sec_vectors[ARMV7M_EXCP_SVC].pending = (value & (1 << 15)) != 0;
            s->sec_vectors[ARMV7M_EXCP_MEM].enabled = (value & (1 << 16)) != 0;
            s->sec_vectors[ARMV7M_EXCP_BUS].enabled = (value & (1 << 17)) != 0;
            s->sec_vectors[ARMV7M_EXCP_USAGE].enabled =
                (value & (1 << 18)) != 0;
            s->sec_vectors[ARMV7M_EXCP_HARD].pending = (value & (1 << 21)) != 0;
            /
            s->vectors[ARMV7M_EXCP_SECURE].active = (value & (1 << 4)) != 0;
            s->vectors[ARMV7M_EXCP_SECURE].enabled = (value & (1 << 19)) != 0;
            s->vectors[ARMV7M_EXCP_SECURE].pending = (value & (1 << 20)) != 0;
        } else {
            s->vectors[ARMV7M_EXCP_MEM].active = (value & (1 << 0)) != 0;
            if (arm_feature(&cpu->env, ARM_FEATURE_V8)) {
                /
                s->vectors[ARMV7M_EXCP_HARD].pending = (value & (1 << 21)) != 0;
            }
            s->vectors[ARMV7M_EXCP_USAGE].active = (value & (1 << 3)) != 0;
            s->vectors[ARMV7M_EXCP_SVC].active = (value & (1 << 7)) != 0;
            s->vectors[ARMV7M_EXCP_PENDSV].active = (value & (1 << 10)) != 0;
            s->vectors[ARMV7M_EXCP_SYSTICK].active = (value & (1 << 11)) != 0;
            s->vectors[ARMV7M_EXCP_USAGE].pending = (value & (1 << 12)) != 0;
            s->vectors[ARMV7M_EXCP_MEM].pending = (value & (1 << 13)) != 0;
            s->vectors[ARMV7M_EXCP_SVC].pending = (value & (1 << 15)) != 0;
            s->vectors[ARMV7M_EXCP_MEM].enabled = (value & (1 << 16)) != 0;
            s->vectors[ARMV7M_EXCP_USAGE].enabled = (value & (1 << 18)) != 0;
        }
        if (attrs.secure || (cpu->env.v7m.aircr & R_V7M_AIRCR_BFHFNMINS_MASK)) {
            s->vectors[ARMV7M_EXCP_BUS].active = (value & (1 << 1)) != 0;
            s->vectors[ARMV7M_EXCP_BUS].pending = (value & (1 << 14)) != 0;
            s->vectors[ARMV7M_EXCP_BUS].enabled = (value & (1 << 17)) != 0;
        }
        /
        if (!attrs.secure && cpu->env.v7m.secure &&
            (cpu->env.v7m.aircr & R_V7M_AIRCR_BFHFNMINS_MASK) &&
            (value & (1 << 5)) == 0) {
            s->vectors[ARMV7M_EXCP_NMI].active = 0;
        }
        /
        if (!attrs.secure && cpu->env.v7m.secure &&
            (cpu->env.v7m.aircr & R_V7M_AIRCR_BFHFNMINS_MASK) &&
            (value & (1 << 2)) == 0) {
            s->vectors[ARMV7M_EXCP_HARD].active = 0;
        }

        /
        s->vectors[ARMV7M_EXCP_DEBUG].active = (value & (1 << 8)) != 0;
        nvic_irq_update(s);
        break;
    case 0xd28: /
        cpu->env.v7m.cfsr[attrs.secure] &= ~value; /
        if (attrs.secure) {
            /
            cpu->env.v7m.cfsr[M_REG_NS] &= ~(value & R_V7M_CFSR_BFSR_MASK);
        }
        break;
    case 0xd2c: /
        cpu->env.v7m.hfsr &= ~value; /
        break;
    case 0xd30: /
        cpu->env.v7m.dfsr &= ~value; /
        break;
    case 0xd34: /
        cpu->env.v7m.mmfar[attrs.secure] = value;
        return;
    case 0xd38: /
        cpu->env.v7m.bfar = value;
        return;
    case 0xd3c: /
        qemu_log_mask(LOG_UNIMP,
                      "NVIC: Aux fault status registers unimplemented\n");
        break;
    case 0xd90: /
        return; /
    case 0xd94: /
        if ((value &
             (R_V7M_MPU_CTRL_HFNMIENA_MASK | R_V7M_MPU_CTRL_ENABLE_MASK))
            == R_V7M_MPU_CTRL_HFNMIENA_MASK) {
            qemu_log_mask(LOG_GUEST_ERROR, "MPU_CTRL: HFNMIENA and !ENABLE is "
                          "UNPREDICTABLE\n");
        }
        cpu->env.v7m.mpu_ctrl[attrs.secure]
            = value & (R_V7M_MPU_CTRL_ENABLE_MASK |
                       R_V7M_MPU_CTRL_HFNMIENA_MASK |
                       R_V7M_MPU_CTRL_PRIVDEFENA_MASK);
        tlb_flush(CPU(cpu));
        break;
    case 0xd98: /
        if (value >= cpu->pmsav7_dregion) {
            qemu_log_mask(LOG_GUEST_ERROR, "MPU region out of range %"
                          PRIu32 "/%" PRIu32 "\n",
                          value, cpu->pmsav7_dregion);
        } else {
            cpu->env.pmsav7.rnr[attrs.secure] = value;
        }
        break;
    case 0xd9c: /
    case 0xda4: /
    case 0xdac: /
    case 0xdb4: /
    {
        int region;

        if (arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            /
            int aliasno = (offset - 0xd9c) / 8; /

            region = cpu->env.pmsav7.rnr[attrs.secure];
            if (aliasno) {
                region = deposit32(region, 0, 2, aliasno);
            }
            if (region >= cpu->pmsav7_dregion) {
                return;
            }
            cpu->env.pmsav8.rbar[attrs.secure][region] = value;
            tlb_flush(CPU(cpu));
            return;
        }

        if (value & (1 << 4)) {
            /
            region = extract32(value, 0, 4);
            if (region >= cpu->pmsav7_dregion) {
                qemu_log_mask(LOG_GUEST_ERROR,
                              "MPU region out of range %u/%" PRIu32 "\n",
                              region, cpu->pmsav7_dregion);
                return;
            }
            cpu->env.pmsav7.rnr[attrs.secure] = region;
        } else {
            region = cpu->env.pmsav7.rnr[attrs.secure];
        }

        if (region >= cpu->pmsav7_dregion) {
            return;
        }

        cpu->env.pmsav7.drbar[region] = value & ~0x1f;
        tlb_flush(CPU(cpu));
        break;
    }
    case 0xda0: /
    case 0xda8: /
    case 0xdb0: /
    case 0xdb8: /
    {
        int region = cpu->env.pmsav7.rnr[attrs.secure];

        if (arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            /
            int aliasno = (offset - 0xd9c) / 8; /

            region = cpu->env.pmsav7.rnr[attrs.secure];
            if (aliasno) {
                region = deposit32(region, 0, 2, aliasno);
            }
            if (region >= cpu->pmsav7_dregion) {
                return;
            }
            cpu->env.pmsav8.rlar[attrs.secure][region] = value;
            tlb_flush(CPU(cpu));
            return;
        }

        if (region >= cpu->pmsav7_dregion) {
            return;
        }

        cpu->env.pmsav7.drsr[region] = value & 0xff3f;
        cpu->env.pmsav7.dracr[region] = (value >> 16) & 0x173f;
        tlb_flush(CPU(cpu));
        break;
    }
    case 0xdc0: /
        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (cpu->pmsav7_dregion) {
            /
            cpu->env.pmsav8.mair0[attrs.secure] = value;
        }
        /
        break;
    case 0xdc4: /
        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (cpu->pmsav7_dregion) {
            /
            cpu->env.pmsav8.mair1[attrs.secure] = value;
        }
        /
        break;
    case 0xdd0: /
        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (!attrs.secure) {
            return;
        }
        cpu->env.sau.ctrl = value & 3;
        break;
    case 0xdd4: /
        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        break;
    case 0xdd8: /
        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (!attrs.secure) {
            return;
        }
        if (value >= cpu->sau_sregion) {
            qemu_log_mask(LOG_GUEST_ERROR, "SAU region out of range %"
                          PRIu32 "/%" PRIu32 "\n",
                          value, cpu->sau_sregion);
        } else {
            cpu->env.sau.rnr = value;
        }
        break;
    case 0xddc: /
    {
        int region = cpu->env.sau.rnr;

        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (!attrs.secure) {
            return;
        }
        if (region >= cpu->sau_sregion) {
            return;
        }
        cpu->env.sau.rbar[region] = value & ~0x1f;
        tlb_flush(CPU(cpu));
        break;
    }
    case 0xde0: /
    {
        int region = cpu->env.sau.rnr;

        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (!attrs.secure) {
            return;
        }
        if (region >= cpu->sau_sregion) {
            return;
        }
        cpu->env.sau.rlar[region] = value & ~0x1c;
        tlb_flush(CPU(cpu));
        break;
    }
    case 0xde4: /
        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (!attrs.secure) {
            return;
        }
        cpu->env.v7m.sfsr &= ~value; /
        break;
    case 0xde8: /
        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {
            goto bad_offset;
        }
        if (!attrs.secure) {
            return;
        }
        cpu->env.v7m.sfsr = value;
        break;
    case 0xf00: /
    {
        int excnum = (value & 0x1ff) + NVIC_FIRST_IRQ;
        if (excnum < s->num_irq) {
            armv7m_nvic_set_pending(s, excnum, false);
        }
        break;
    }
    default:
    bad_offset:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "NVIC: Bad write offset 0x%x\n", offset);
    }
}
