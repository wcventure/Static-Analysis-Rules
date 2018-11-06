static void gen_sse(DisasContext *s, int b, target_ulong pc_start, int rex_r)
{
    int b1, op1_offset, op2_offset, is_xmm, val, ot;
    int modrm, mod, rm, reg, reg_addr, offset_addr;
    SSEFunc_0_pp sse_fn_pp;
    SSEFunc_0_ppi sse_fn_ppi;
    SSEFunc_0_ppt sse_fn_ppt;

    b &= 0xff;
    if (s->prefix & PREFIX_DATA)
        b1 = 1;
    else if (s->prefix & PREFIX_REPZ)
        b1 = 2;
    else if (s->prefix & PREFIX_REPNZ)
        b1 = 3;
    else
        b1 = 0;
    sse_fn_pp = sse_op_table1[b][b1];
    if (!sse_fn_pp) {
        goto illegal_op;
    }
    if ((b <= 0x5f && b >= 0x10) || b == 0xc6 || b == 0xc2) {
        is_xmm = 1;
    } else {
        if (b1 == 0) {
            /
            is_xmm = 0;
        } else {
            is_xmm = 1;
        }
    }
    /
    if (s->flags & HF_TS_MASK) {
        gen_exception(s, EXCP07_PREX, pc_start - s->cs_base);
        return;
    }
    if (s->flags & HF_EM_MASK) {
    illegal_op:
        gen_exception(s, EXCP06_ILLOP, pc_start - s->cs_base);
        return;
    }
    if (is_xmm && !(s->flags & HF_OSFXSR_MASK))
        if ((b != 0x38 && b != 0x3a) || (s->prefix & PREFIX_DATA))
            goto illegal_op;
    if (b == 0x0e) {
        if (!(s->cpuid_ext2_features & CPUID_EXT2_3DNOW))
            goto illegal_op;
        /
        gen_helper_emms();
        return;
    }
    if (b == 0x77) {
        /
        gen_helper_emms();
        return;
    }
    /
    if (!is_xmm) {
        gen_helper_enter_mmx();
    }

    modrm = ldub_code(s->pc++);
    reg = ((modrm >> 3) & 7);
    if (is_xmm)
        reg |= rex_r;
    mod = (modrm >> 6) & 3;
    if (sse_fn_pp == SSE_SPECIAL) {
        b |= (b1 << 8);
        switch(b) {
        case 0x0e7: /
            if (mod == 3)
                goto illegal_op;
            gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
            gen_stq_env_A0(s->mem_index, offsetof(CPUX86State,fpregs[reg].mmx));
            break;
        case 0x1e7: /
        case 0x02b: /
        case 0x12b: /
            if (mod == 3)
                goto illegal_op;
            gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
            gen_sto_env_A0(s->mem_index, offsetof(CPUX86State,xmm_regs[reg]));
            break;
        case 0x3f0: /
            if (mod == 3)
                goto illegal_op;
            gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
            gen_ldo_env_A0(s->mem_index, offsetof(CPUX86State,xmm_regs[reg]));
            break;
        case 0x22b: /
        case 0x32b: /
            if (mod == 3)
                goto illegal_op;
            gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
            if (b1 & 1) {
                gen_stq_env_A0(s->mem_index, offsetof(CPUX86State,
                    xmm_regs[reg]));
            } else {
                tcg_gen_ld32u_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,
                    xmm_regs[reg].XMM_L(0)));
                gen_op_st_T0_A0(OT_LONG + s->mem_index);
            }
            break;
        case 0x6e: /
#ifdef TARGET_X86_64
            if (s->dflag == 2) {
                gen_ldst_modrm(s, modrm, OT_QUAD, OR_TMP0, 0);
                tcg_gen_st_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,fpregs[reg].mmx));
            } else
#endif
            {
                gen_ldst_modrm(s, modrm, OT_LONG, OR_TMP0, 0);
                tcg_gen_addi_ptr(cpu_ptr0, cpu_env, 
                                 offsetof(CPUX86State,fpregs[reg].mmx));
                tcg_gen_trunc_tl_i32(cpu_tmp2_i32, cpu_T[0]);
                gen_helper_movl_mm_T0_mmx(cpu_ptr0, cpu_tmp2_i32);
            }
            break;
        case 0x16e: /
#ifdef TARGET_X86_64
            if (s->dflag == 2) {
                gen_ldst_modrm(s, modrm, OT_QUAD, OR_TMP0, 0);
                tcg_gen_addi_ptr(cpu_ptr0, cpu_env, 
                                 offsetof(CPUX86State,xmm_regs[reg]));
                gen_helper_movq_mm_T0_xmm(cpu_ptr0, cpu_T[0]);
            } else
#endif
            {
                gen_ldst_modrm(s, modrm, OT_LONG, OR_TMP0, 0);
                tcg_gen_addi_ptr(cpu_ptr0, cpu_env, 
                                 offsetof(CPUX86State,xmm_regs[reg]));
                tcg_gen_trunc_tl_i32(cpu_tmp2_i32, cpu_T[0]);
                gen_helper_movl_mm_T0_xmm(cpu_ptr0, cpu_tmp2_i32);
            }
            break;
        case 0x6f: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                gen_ldq_env_A0(s->mem_index, offsetof(CPUX86State,fpregs[reg].mmx));
            } else {
                rm = (modrm & 7);
                tcg_gen_ld_i64(cpu_tmp1_i64, cpu_env,
                               offsetof(CPUX86State,fpregs[rm].mmx));
                tcg_gen_st_i64(cpu_tmp1_i64, cpu_env,
                               offsetof(CPUX86State,fpregs[reg].mmx));
            }
            break;
        case 0x010: /
        case 0x110: /
        case 0x028: /
        case 0x128: /
        case 0x16f: /
        case 0x26f: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                gen_ldo_env_A0(s->mem_index, offsetof(CPUX86State,xmm_regs[reg]));
            } else {
                rm = (modrm & 7) | REX_B(s);
                gen_op_movo(offsetof(CPUX86State,xmm_regs[reg]),
                            offsetof(CPUX86State,xmm_regs[rm]));
            }
            break;
        case 0x210: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                gen_op_ld_T0_A0(OT_LONG + s->mem_index);
                tcg_gen_st32_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,xmm_regs[reg].XMM_L(0)));
                gen_op_movl_T0_0();
                tcg_gen_st32_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,xmm_regs[reg].XMM_L(1)));
                tcg_gen_st32_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,xmm_regs[reg].XMM_L(2)));
                tcg_gen_st32_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,xmm_regs[reg].XMM_L(3)));
            } else {
                rm = (modrm & 7) | REX_B(s);
                gen_op_movl(offsetof(CPUX86State,xmm_regs[reg].XMM_L(0)),
                            offsetof(CPUX86State,xmm_regs[rm].XMM_L(0)));
            }
            break;
        case 0x310: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                gen_ldq_env_A0(s->mem_index, offsetof(CPUX86State,xmm_regs[reg].XMM_Q(0)));
                gen_op_movl_T0_0();
                tcg_gen_st32_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,xmm_regs[reg].XMM_L(2)));
                tcg_gen_st32_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,xmm_regs[reg].XMM_L(3)));
            } else {
                rm = (modrm & 7) | REX_B(s);
                gen_op_movq(offsetof(CPUX86State,xmm_regs[reg].XMM_Q(0)),
                            offsetof(CPUX86State,xmm_regs[rm].XMM_Q(0)));
            }
            break;
        case 0x012: /
        case 0x112: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                gen_ldq_env_A0(s->mem_index, offsetof(CPUX86State,xmm_regs[reg].XMM_Q(0)));
            } else {
                /
                rm = (modrm & 7) | REX_B(s);
                gen_op_movq(offsetof(CPUX86State,xmm_regs[reg].XMM_Q(0)),
                            offsetof(CPUX86State,xmm_regs[rm].XMM_Q(1)));
            }
            break;
        case 0x212: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                gen_ldo_env_A0(s->mem_index, offsetof(CPUX86State,xmm_regs[reg]));
            } else {
                rm = (modrm & 7) | REX_B(s);
                gen_op_movl(offsetof(CPUX86State,xmm_regs[reg].XMM_L(0)),
                            offsetof(CPUX86State,xmm_regs[rm].XMM_L(0)));
                gen_op_movl(offsetof(CPUX86State,xmm_regs[reg].XMM_L(2)),
                            offsetof(CPUX86State,xmm_regs[rm].XMM_L(2)));
            }
            gen_op_movl(offsetof(CPUX86State,xmm_regs[reg].XMM_L(1)),
                        offsetof(CPUX86State,xmm_regs[reg].XMM_L(0)));
            gen_op_movl(offsetof(CPUX86State,xmm_regs[reg].XMM_L(3)),
                        offsetof(CPUX86State,xmm_regs[reg].XMM_L(2)));
            break;
        case 0x312: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                gen_ldq_env_A0(s->mem_index, offsetof(CPUX86State,xmm_regs[reg].XMM_Q(0)));
            } else {
                rm = (modrm & 7) | REX_B(s);
                gen_op_movq(offsetof(CPUX86State,xmm_regs[reg].XMM_Q(0)),
                            offsetof(CPUX86State,xmm_regs[rm].XMM_Q(0)));
            }
            gen_op_movq(offsetof(CPUX86State,xmm_regs[reg].XMM_Q(1)),
                        offsetof(CPUX86State,xmm_regs[reg].XMM_Q(0)));
            break;
        case 0x016: /
        case 0x116: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                gen_ldq_env_A0(s->mem_index, offsetof(CPUX86State,xmm_regs[reg].XMM_Q(1)));
            } else {
                /
                rm = (modrm & 7) | REX_B(s);
                gen_op_movq(offsetof(CPUX86State,xmm_regs[reg].XMM_Q(1)),
                            offsetof(CPUX86State,xmm_regs[rm].XMM_Q(0)));
            }
            break;
        case 0x216: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                gen_ldo_env_A0(s->mem_index, offsetof(CPUX86State,xmm_regs[reg]));
            } else {
                rm = (modrm & 7) | REX_B(s);
                gen_op_movl(offsetof(CPUX86State,xmm_regs[reg].XMM_L(1)),
                            offsetof(CPUX86State,xmm_regs[rm].XMM_L(1)));
                gen_op_movl(offsetof(CPUX86State,xmm_regs[reg].XMM_L(3)),
                            offsetof(CPUX86State,xmm_regs[rm].XMM_L(3)));
            }
            gen_op_movl(offsetof(CPUX86State,xmm_regs[reg].XMM_L(0)),
                        offsetof(CPUX86State,xmm_regs[reg].XMM_L(1)));
            gen_op_movl(offsetof(CPUX86State,xmm_regs[reg].XMM_L(2)),
                        offsetof(CPUX86State,xmm_regs[reg].XMM_L(3)));
            break;
        case 0x178:
        case 0x378:
            {
                int bit_index, field_length;

                if (b1 == 1 && reg != 0)
                    goto illegal_op;
                field_length = ldub_code(s->pc++) & 0x3F;
                bit_index = ldub_code(s->pc++) & 0x3F;
                tcg_gen_addi_ptr(cpu_ptr0, cpu_env,
                    offsetof(CPUX86State,xmm_regs[reg]));
                if (b1 == 1)
                    gen_helper_extrq_i(cpu_ptr0, tcg_const_i32(bit_index),
                        tcg_const_i32(field_length));
                else
                    gen_helper_insertq_i(cpu_ptr0, tcg_const_i32(bit_index),
                        tcg_const_i32(field_length));
            }
            break;
        case 0x7e: /
#ifdef TARGET_X86_64
            if (s->dflag == 2) {
                tcg_gen_ld_i64(cpu_T[0], cpu_env, 
                               offsetof(CPUX86State,fpregs[reg].mmx));
                gen_ldst_modrm(s, modrm, OT_QUAD, OR_TMP0, 1);
            } else
#endif
            {
                tcg_gen_ld32u_tl(cpu_T[0], cpu_env, 
                                 offsetof(CPUX86State,fpregs[reg].mmx.MMX_L(0)));
                gen_ldst_modrm(s, modrm, OT_LONG, OR_TMP0, 1);
            }
            break;
        case 0x17e: /
#ifdef TARGET_X86_64
            if (s->dflag == 2) {
                tcg_gen_ld_i64(cpu_T[0], cpu_env, 
                               offsetof(CPUX86State,xmm_regs[reg].XMM_Q(0)));
                gen_ldst_modrm(s, modrm, OT_QUAD, OR_TMP0, 1);
            } else
#endif
            {
                tcg_gen_ld32u_tl(cpu_T[0], cpu_env, 
                                 offsetof(CPUX86State,xmm_regs[reg].XMM_L(0)));
                gen_ldst_modrm(s, modrm, OT_LONG, OR_TMP0, 1);
            }
            break;
        case 0x27e: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                gen_ldq_env_A0(s->mem_index, offsetof(CPUX86State,xmm_regs[reg].XMM_Q(0)));
            } else {
                rm = (modrm & 7) | REX_B(s);
                gen_op_movq(offsetof(CPUX86State,xmm_regs[reg].XMM_Q(0)),
                            offsetof(CPUX86State,xmm_regs[rm].XMM_Q(0)));
            }
            gen_op_movq_env_0(offsetof(CPUX86State,xmm_regs[reg].XMM_Q(1)));
            break;
        case 0x7f: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                gen_stq_env_A0(s->mem_index, offsetof(CPUX86State,fpregs[reg].mmx));
            } else {
                rm = (modrm & 7);
                gen_op_movq(offsetof(CPUX86State,fpregs[rm].mmx),
                            offsetof(CPUX86State,fpregs[reg].mmx));
            }
            break;
        case 0x011: /
        case 0x111: /
        case 0x029: /
        case 0x129: /
        case 0x17f: /
        case 0x27f: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                gen_sto_env_A0(s->mem_index, offsetof(CPUX86State,xmm_regs[reg]));
            } else {
                rm = (modrm & 7) | REX_B(s);
                gen_op_movo(offsetof(CPUX86State,xmm_regs[rm]),
                            offsetof(CPUX86State,xmm_regs[reg]));
            }
            break;
        case 0x211: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                tcg_gen_ld32u_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,xmm_regs[reg].XMM_L(0)));
                gen_op_st_T0_A0(OT_LONG + s->mem_index);
            } else {
                rm = (modrm & 7) | REX_B(s);
                gen_op_movl(offsetof(CPUX86State,xmm_regs[rm].XMM_L(0)),
                            offsetof(CPUX86State,xmm_regs[reg].XMM_L(0)));
            }
            break;
        case 0x311: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                gen_stq_env_A0(s->mem_index, offsetof(CPUX86State,xmm_regs[reg].XMM_Q(0)));
            } else {
                rm = (modrm & 7) | REX_B(s);
                gen_op_movq(offsetof(CPUX86State,xmm_regs[rm].XMM_Q(0)),
                            offsetof(CPUX86State,xmm_regs[reg].XMM_Q(0)));
            }
            break;
        case 0x013: /
        case 0x113: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                gen_stq_env_A0(s->mem_index, offsetof(CPUX86State,xmm_regs[reg].XMM_Q(0)));
            } else {
                goto illegal_op;
            }
            break;
        case 0x017: /
        case 0x117: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                gen_stq_env_A0(s->mem_index, offsetof(CPUX86State,xmm_regs[reg].XMM_Q(1)));
            } else {
                goto illegal_op;
            }
            break;
        case 0x71: /
        case 0x72:
        case 0x73:
        case 0x171: /
        case 0x172:
        case 0x173:
            if (b1 >= 2) {
	        goto illegal_op;
            }
            val = ldub_code(s->pc++);
            if (is_xmm) {
                gen_op_movl_T0_im(val);
                tcg_gen_st32_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,xmm_t0.XMM_L(0)));
                gen_op_movl_T0_0();
                tcg_gen_st32_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,xmm_t0.XMM_L(1)));
                op1_offset = offsetof(CPUX86State,xmm_t0);
            } else {
                gen_op_movl_T0_im(val);
                tcg_gen_st32_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,mmx_t0.MMX_L(0)));
                gen_op_movl_T0_0();
                tcg_gen_st32_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,mmx_t0.MMX_L(1)));
                op1_offset = offsetof(CPUX86State,mmx_t0);
            }
            sse_fn_pp = sse_op_table2[((b - 1) & 3) * 8 + (((modrm >> 3)) & 7)][b1];
            if (!sse_fn_pp) {
                goto illegal_op;
            }
            if (is_xmm) {
                rm = (modrm & 7) | REX_B(s);
                op2_offset = offsetof(CPUX86State,xmm_regs[rm]);
            } else {
                rm = (modrm & 7);
                op2_offset = offsetof(CPUX86State,fpregs[rm].mmx);
            }
            tcg_gen_addi_ptr(cpu_ptr0, cpu_env, op2_offset);
            tcg_gen_addi_ptr(cpu_ptr1, cpu_env, op1_offset);
            sse_fn_pp(cpu_ptr0, cpu_ptr1);
            break;
        case 0x050: /
            rm = (modrm & 7) | REX_B(s);
            tcg_gen_addi_ptr(cpu_ptr0, cpu_env, 
                             offsetof(CPUX86State,xmm_regs[rm]));
            gen_helper_movmskps(cpu_tmp2_i32, cpu_ptr0);
            tcg_gen_extu_i32_tl(cpu_T[0], cpu_tmp2_i32);
            gen_op_mov_reg_T0(OT_LONG, reg);
            break;
        case 0x150: /
            rm = (modrm & 7) | REX_B(s);
            tcg_gen_addi_ptr(cpu_ptr0, cpu_env, 
                             offsetof(CPUX86State,xmm_regs[rm]));
            gen_helper_movmskpd(cpu_tmp2_i32, cpu_ptr0);
            tcg_gen_extu_i32_tl(cpu_T[0], cpu_tmp2_i32);
            gen_op_mov_reg_T0(OT_LONG, reg);
            break;
        case 0x02a: /
        case 0x12a: /
            gen_helper_enter_mmx();
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                op2_offset = offsetof(CPUX86State,mmx_t0);
                gen_ldq_env_A0(s->mem_index, op2_offset);
            } else {
                rm = (modrm & 7);
                op2_offset = offsetof(CPUX86State,fpregs[rm].mmx);
            }
            op1_offset = offsetof(CPUX86State,xmm_regs[reg]);
            tcg_gen_addi_ptr(cpu_ptr0, cpu_env, op1_offset);
            tcg_gen_addi_ptr(cpu_ptr1, cpu_env, op2_offset);
            switch(b >> 8) {
            case 0x0:
                gen_helper_cvtpi2ps(cpu_ptr0, cpu_ptr1);
                break;
            default:
            case 0x1:
                gen_helper_cvtpi2pd(cpu_ptr0, cpu_ptr1);
                break;
            }
            break;
        case 0x22a: /
        case 0x32a: /
            ot = (s->dflag == 2) ? OT_QUAD : OT_LONG;
            gen_ldst_modrm(s, modrm, ot, OR_TMP0, 0);
            op1_offset = offsetof(CPUX86State,xmm_regs[reg]);
            tcg_gen_addi_ptr(cpu_ptr0, cpu_env, op1_offset);
            if (ot == OT_LONG) {
                SSEFunc_0_pi sse_fn_pi = sse_op_table3ai[(b >> 8) - 2];
                tcg_gen_trunc_tl_i32(cpu_tmp2_i32, cpu_T[0]);
                sse_fn_pi(cpu_ptr0, cpu_tmp2_i32);
            } else {
#ifdef TARGET_X86_64
                SSEFunc_0_pl sse_fn_pl = sse_op_table3aq[(b >> 8) - 2];
                sse_fn_pl(cpu_ptr0, cpu_T[0]);
#else
                goto illegal_op;
#endif
            }
            break;
        case 0x02c: /
        case 0x12c: /
        case 0x02d: /
        case 0x12d: /
            gen_helper_enter_mmx();
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                op2_offset = offsetof(CPUX86State,xmm_t0);
                gen_ldo_env_A0(s->mem_index, op2_offset);
            } else {
                rm = (modrm & 7) | REX_B(s);
                op2_offset = offsetof(CPUX86State,xmm_regs[rm]);
            }
            op1_offset = offsetof(CPUX86State,fpregs[reg & 7].mmx);
            tcg_gen_addi_ptr(cpu_ptr0, cpu_env, op1_offset);
            tcg_gen_addi_ptr(cpu_ptr1, cpu_env, op2_offset);
            switch(b) {
            case 0x02c:
                gen_helper_cvttps2pi(cpu_ptr0, cpu_ptr1);
                break;
            case 0x12c:
                gen_helper_cvttpd2pi(cpu_ptr0, cpu_ptr1);
                break;
            case 0x02d:
                gen_helper_cvtps2pi(cpu_ptr0, cpu_ptr1);
                break;
            case 0x12d:
                gen_helper_cvtpd2pi(cpu_ptr0, cpu_ptr1);
                break;
            }
            break;
        case 0x22c: /
        case 0x32c: /
        case 0x22d: /
        case 0x32d: /
            ot = (s->dflag == 2) ? OT_QUAD : OT_LONG;
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                if ((b >> 8) & 1) {
                    gen_ldq_env_A0(s->mem_index, offsetof(CPUX86State,xmm_t0.XMM_Q(0)));
                } else {
                    gen_op_ld_T0_A0(OT_LONG + s->mem_index);
                    tcg_gen_st32_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,xmm_t0.XMM_L(0)));
                }
                op2_offset = offsetof(CPUX86State,xmm_t0);
            } else {
                rm = (modrm & 7) | REX_B(s);
                op2_offset = offsetof(CPUX86State,xmm_regs[rm]);
            }
            tcg_gen_addi_ptr(cpu_ptr0, cpu_env, op2_offset);
            if (ot == OT_LONG) {
                SSEFunc_i_p sse_fn_i_p =
                    sse_op_table3bi[(b >> 8) - 2 + (b & 1) * 2];
                sse_fn_i_p(cpu_tmp2_i32, cpu_ptr0);
                tcg_gen_extu_i32_tl(cpu_T[0], cpu_tmp2_i32);
            } else {
#ifdef TARGET_X86_64
                SSEFunc_l_p sse_fn_l_p =
                    sse_op_table3bq[(b >> 8) - 2 + (b & 1) * 2];
                sse_fn_l_p(cpu_T[0], cpu_ptr0);
#else
                goto illegal_op;
#endif
            }
            gen_op_mov_reg_T0(ot, reg);
            break;
        case 0xc4: /
        case 0x1c4:
            s->rip_offset = 1;
            gen_ldst_modrm(s, modrm, OT_WORD, OR_TMP0, 0);
            val = ldub_code(s->pc++);
            if (b1) {
                val &= 7;
                tcg_gen_st16_tl(cpu_T[0], cpu_env,
                                offsetof(CPUX86State,xmm_regs[reg].XMM_W(val)));
            } else {
                val &= 3;
                tcg_gen_st16_tl(cpu_T[0], cpu_env,
                                offsetof(CPUX86State,fpregs[reg].mmx.MMX_W(val)));
            }
            break;
        case 0xc5: /
        case 0x1c5:
            if (mod != 3)
                goto illegal_op;
            ot = (s->dflag == 2) ? OT_QUAD : OT_LONG;
            val = ldub_code(s->pc++);
            if (b1) {
                val &= 7;
                rm = (modrm & 7) | REX_B(s);
                tcg_gen_ld16u_tl(cpu_T[0], cpu_env,
                                 offsetof(CPUX86State,xmm_regs[rm].XMM_W(val)));
            } else {
                val &= 3;
                rm = (modrm & 7);
                tcg_gen_ld16u_tl(cpu_T[0], cpu_env,
                                offsetof(CPUX86State,fpregs[rm].mmx.MMX_W(val)));
            }
            reg = ((modrm >> 3) & 7) | rex_r;
            gen_op_mov_reg_T0(ot, reg);
            break;
        case 0x1d6: /
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                gen_stq_env_A0(s->mem_index, offsetof(CPUX86State,xmm_regs[reg].XMM_Q(0)));
            } else {
                rm = (modrm & 7) | REX_B(s);
                gen_op_movq(offsetof(CPUX86State,xmm_regs[rm].XMM_Q(0)),
                            offsetof(CPUX86State,xmm_regs[reg].XMM_Q(0)));
                gen_op_movq_env_0(offsetof(CPUX86State,xmm_regs[rm].XMM_Q(1)));
            }
            break;
        case 0x2d6: /
            gen_helper_enter_mmx();
            rm = (modrm & 7);
            gen_op_movq(offsetof(CPUX86State,xmm_regs[reg].XMM_Q(0)),
                        offsetof(CPUX86State,fpregs[rm].mmx));
            gen_op_movq_env_0(offsetof(CPUX86State,xmm_regs[reg].XMM_Q(1)));
            break;
        case 0x3d6: /
            gen_helper_enter_mmx();
            rm = (modrm & 7) | REX_B(s);
            gen_op_movq(offsetof(CPUX86State,fpregs[reg & 7].mmx),
                        offsetof(CPUX86State,xmm_regs[rm].XMM_Q(0)));
            break;
        case 0xd7: /
        case 0x1d7:
            if (mod != 3)
                goto illegal_op;
            if (b1) {
                rm = (modrm & 7) | REX_B(s);
                tcg_gen_addi_ptr(cpu_ptr0, cpu_env, offsetof(CPUX86State,xmm_regs[rm]));
                gen_helper_pmovmskb_xmm(cpu_tmp2_i32, cpu_ptr0);
            } else {
                rm = (modrm & 7);
                tcg_gen_addi_ptr(cpu_ptr0, cpu_env, offsetof(CPUX86State,fpregs[rm].mmx));
                gen_helper_pmovmskb_mmx(cpu_tmp2_i32, cpu_ptr0);
            }
            tcg_gen_extu_i32_tl(cpu_T[0], cpu_tmp2_i32);
            reg = ((modrm >> 3) & 7) | rex_r;
            gen_op_mov_reg_T0(OT_LONG, reg);
            break;
        case 0x138:
            if (s->prefix & PREFIX_REPNZ)
                goto crc32;
        case 0x038:
            b = modrm;
            modrm = ldub_code(s->pc++);
            rm = modrm & 7;
            reg = ((modrm >> 3) & 7) | rex_r;
            mod = (modrm >> 6) & 3;
            if (b1 >= 2) {
                goto illegal_op;
            }

            sse_fn_pp = sse_op_table6[b].op[b1];
            if (!sse_fn_pp) {
                goto illegal_op;
            }
            if (!(s->cpuid_ext_features & sse_op_table6[b].ext_mask))
                goto illegal_op;

            if (b1) {
                op1_offset = offsetof(CPUX86State,xmm_regs[reg]);
                if (mod == 3) {
                    op2_offset = offsetof(CPUX86State,xmm_regs[rm | REX_B(s)]);
                } else {
                    op2_offset = offsetof(CPUX86State,xmm_t0);
                    gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                    switch (b) {
                    case 0x20: case 0x30: /
                    case 0x23: case 0x33: /
                    case 0x25: case 0x35: /
                        gen_ldq_env_A0(s->mem_index, op2_offset +
                                        offsetof(XMMReg, XMM_Q(0)));
                        break;
                    case 0x21: case 0x31: /
                    case 0x24: case 0x34: /
                        tcg_gen_qemu_ld32u(cpu_tmp0, cpu_A0,
                                          (s->mem_index >> 2) - 1);
                        tcg_gen_trunc_tl_i32(cpu_tmp2_i32, cpu_tmp0);
                        tcg_gen_st_i32(cpu_tmp2_i32, cpu_env, op2_offset +
                                        offsetof(XMMReg, XMM_L(0)));
                        break;
                    case 0x22: case 0x32: /
                        tcg_gen_qemu_ld16u(cpu_tmp0, cpu_A0,
                                          (s->mem_index >> 2) - 1);
                        tcg_gen_st16_tl(cpu_tmp0, cpu_env, op2_offset +
                                        offsetof(XMMReg, XMM_W(0)));
                        break;
                    case 0x2a:            /
                        gen_ldo_env_A0(s->mem_index, op1_offset);
                        return;
                    default:
                        gen_ldo_env_A0(s->mem_index, op2_offset);
                    }
                }
            } else {
                op1_offset = offsetof(CPUX86State,fpregs[reg].mmx);
                if (mod == 3) {
                    op2_offset = offsetof(CPUX86State,fpregs[rm].mmx);
                } else {
                    op2_offset = offsetof(CPUX86State,mmx_t0);
                    gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                    gen_ldq_env_A0(s->mem_index, op2_offset);
                }
            }
            if (sse_fn_pp == SSE_SPECIAL) {
                goto illegal_op;
            }

            tcg_gen_addi_ptr(cpu_ptr0, cpu_env, op1_offset);
            tcg_gen_addi_ptr(cpu_ptr1, cpu_env, op2_offset);
            sse_fn_pp(cpu_ptr0, cpu_ptr1);

            if (b == 0x17)
                s->cc_op = CC_OP_EFLAGS;
            break;
        case 0x338: /
        crc32:
            b = modrm;
            modrm = ldub_code(s->pc++);
            reg = ((modrm >> 3) & 7) | rex_r;

            if (b != 0xf0 && b != 0xf1)
                goto illegal_op;
            if (!(s->cpuid_ext_features & CPUID_EXT_SSE42))
                goto illegal_op;

            if (b == 0xf0)
                ot = OT_BYTE;
            else if (b == 0xf1 && s->dflag != 2)
                if (s->prefix & PREFIX_DATA)
                    ot = OT_WORD;
                else
                    ot = OT_LONG;
            else
                ot = OT_QUAD;

            gen_op_mov_TN_reg(OT_LONG, 0, reg);
            tcg_gen_trunc_tl_i32(cpu_tmp2_i32, cpu_T[0]);
            gen_ldst_modrm(s, modrm, ot, OR_TMP0, 0);
            gen_helper_crc32(cpu_T[0], cpu_tmp2_i32,
                             cpu_T[0], tcg_const_i32(8 << ot));

            ot = (s->dflag == 2) ? OT_QUAD : OT_LONG;
            gen_op_mov_reg_T0(ot, reg);
            break;
        case 0x03a:
        case 0x13a:
            b = modrm;
            modrm = ldub_code(s->pc++);
            rm = modrm & 7;
            reg = ((modrm >> 3) & 7) | rex_r;
            mod = (modrm >> 6) & 3;
            if (b1 >= 2) {
                goto illegal_op;
            }

            sse_fn_ppi = sse_op_table7[b].op[b1];
            if (!sse_fn_ppi) {
                goto illegal_op;
            }
            if (!(s->cpuid_ext_features & sse_op_table7[b].ext_mask))
                goto illegal_op;

            if (sse_fn_ppi == SSE_SPECIAL) {
                ot = (s->dflag == 2) ? OT_QUAD : OT_LONG;
                rm = (modrm & 7) | REX_B(s);
                if (mod != 3)
                    gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                reg = ((modrm >> 3) & 7) | rex_r;
                val = ldub_code(s->pc++);
                switch (b) {
                case 0x14: /
                    tcg_gen_ld8u_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,
                                            xmm_regs[reg].XMM_B(val & 15)));
                    if (mod == 3)
                        gen_op_mov_reg_T0(ot, rm);
                    else
                        tcg_gen_qemu_st8(cpu_T[0], cpu_A0,
                                        (s->mem_index >> 2) - 1);
                    break;
                case 0x15: /
                    tcg_gen_ld16u_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,
                                            xmm_regs[reg].XMM_W(val & 7)));
                    if (mod == 3)
                        gen_op_mov_reg_T0(ot, rm);
                    else
                        tcg_gen_qemu_st16(cpu_T[0], cpu_A0,
                                        (s->mem_index >> 2) - 1);
                    break;
                case 0x16:
                    if (ot == OT_LONG) { /
                        tcg_gen_ld_i32(cpu_tmp2_i32, cpu_env,
                                        offsetof(CPUX86State,
                                                xmm_regs[reg].XMM_L(val & 3)));
                        tcg_gen_extu_i32_tl(cpu_T[0], cpu_tmp2_i32);
                        if (mod == 3)
                            gen_op_mov_reg_v(ot, rm, cpu_T[0]);
                        else
                            tcg_gen_qemu_st32(cpu_T[0], cpu_A0,
                                            (s->mem_index >> 2) - 1);
                    } else { /
#ifdef TARGET_X86_64
                        tcg_gen_ld_i64(cpu_tmp1_i64, cpu_env,
                                        offsetof(CPUX86State,
                                                xmm_regs[reg].XMM_Q(val & 1)));
                        if (mod == 3)
                            gen_op_mov_reg_v(ot, rm, cpu_tmp1_i64);
                        else
                            tcg_gen_qemu_st64(cpu_tmp1_i64, cpu_A0,
                                            (s->mem_index >> 2) - 1);
#else
                        goto illegal_op;
#endif
                    }
                    break;
                case 0x17: /
                    tcg_gen_ld32u_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,
                                            xmm_regs[reg].XMM_L(val & 3)));
                    if (mod == 3)
                        gen_op_mov_reg_T0(ot, rm);
                    else
                        tcg_gen_qemu_st32(cpu_T[0], cpu_A0,
                                        (s->mem_index >> 2) - 1);
                    break;
                case 0x20: /
                    if (mod == 3)
                        gen_op_mov_TN_reg(OT_LONG, 0, rm);
                    else
                        tcg_gen_qemu_ld8u(cpu_tmp0, cpu_A0,
                                        (s->mem_index >> 2) - 1);
                    tcg_gen_st8_tl(cpu_tmp0, cpu_env, offsetof(CPUX86State,
                                            xmm_regs[reg].XMM_B(val & 15)));
                    break;
                case 0x21: /
                    if (mod == 3) {
                        tcg_gen_ld_i32(cpu_tmp2_i32, cpu_env,
                                        offsetof(CPUX86State,xmm_regs[rm]
                                                .XMM_L((val >> 6) & 3)));
                    } else {
                        tcg_gen_qemu_ld32u(cpu_tmp0, cpu_A0,
                                        (s->mem_index >> 2) - 1);
                        tcg_gen_trunc_tl_i32(cpu_tmp2_i32, cpu_tmp0);
                    }
                    tcg_gen_st_i32(cpu_tmp2_i32, cpu_env,
                                    offsetof(CPUX86State,xmm_regs[reg]
                                            .XMM_L((val >> 4) & 3)));
                    if ((val >> 0) & 1)
                        tcg_gen_st_i32(tcg_const_i32(0 /),
                                        cpu_env, offsetof(CPUX86State,
                                                xmm_regs[reg].XMM_L(0)));
                    if ((val >> 1) & 1)
                        tcg_gen_st_i32(tcg_const_i32(0 /),
                                        cpu_env, offsetof(CPUX86State,
                                                xmm_regs[reg].XMM_L(1)));
                    if ((val >> 2) & 1)
                        tcg_gen_st_i32(tcg_const_i32(0 /),
                                        cpu_env, offsetof(CPUX86State,
                                                xmm_regs[reg].XMM_L(2)));
                    if ((val >> 3) & 1)
                        tcg_gen_st_i32(tcg_const_i32(0 /),
                                        cpu_env, offsetof(CPUX86State,
                                                xmm_regs[reg].XMM_L(3)));
                    break;
                case 0x22:
                    if (ot == OT_LONG) { /
                        if (mod == 3)
                            gen_op_mov_v_reg(ot, cpu_tmp0, rm);
                        else
                            tcg_gen_qemu_ld32u(cpu_tmp0, cpu_A0,
                                            (s->mem_index >> 2) - 1);
                        tcg_gen_trunc_tl_i32(cpu_tmp2_i32, cpu_tmp0);
                        tcg_gen_st_i32(cpu_tmp2_i32, cpu_env,
                                        offsetof(CPUX86State,
                                                xmm_regs[reg].XMM_L(val & 3)));
                    } else { /
#ifdef TARGET_X86_64
                        if (mod == 3)
                            gen_op_mov_v_reg(ot, cpu_tmp1_i64, rm);
                        else
                            tcg_gen_qemu_ld64(cpu_tmp1_i64, cpu_A0,
                                            (s->mem_index >> 2) - 1);
                        tcg_gen_st_i64(cpu_tmp1_i64, cpu_env,
                                        offsetof(CPUX86State,
                                                xmm_regs[reg].XMM_Q(val & 1)));
#else
                        goto illegal_op;
#endif
                    }
                    break;
                }
                return;
            }

            if (b1) {
                op1_offset = offsetof(CPUX86State,xmm_regs[reg]);
                if (mod == 3) {
                    op2_offset = offsetof(CPUX86State,xmm_regs[rm | REX_B(s)]);
                } else {
                    op2_offset = offsetof(CPUX86State,xmm_t0);
                    gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                    gen_ldo_env_A0(s->mem_index, op2_offset);
                }
            } else {
                op1_offset = offsetof(CPUX86State,fpregs[reg].mmx);
                if (mod == 3) {
                    op2_offset = offsetof(CPUX86State,fpregs[rm].mmx);
                } else {
                    op2_offset = offsetof(CPUX86State,mmx_t0);
                    gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                    gen_ldq_env_A0(s->mem_index, op2_offset);
                }
            }
            val = ldub_code(s->pc++);

            if ((b & 0xfc) == 0x60) { /
                s->cc_op = CC_OP_EFLAGS;

                if (s->dflag == 2)
                    /
                    val |= 1 << 8;
            }

            tcg_gen_addi_ptr(cpu_ptr0, cpu_env, op1_offset);
            tcg_gen_addi_ptr(cpu_ptr1, cpu_env, op2_offset);
            sse_fn_ppi(cpu_ptr0, cpu_ptr1, tcg_const_i32(val));
            break;
        default:
            goto illegal_op;
        }
    } else {
        /
        switch(b) {
        case 0x70: /
        case 0xc6: /
        case 0xc2: /
            s->rip_offset = 1;
            break;
        default:
            break;
        }
        if (is_xmm) {
            op1_offset = offsetof(CPUX86State,xmm_regs[reg]);
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                op2_offset = offsetof(CPUX86State,xmm_t0);
                if (b1 >= 2 && ((b >= 0x50 && b <= 0x5f && b != 0x5b) ||
                                b == 0xc2)) {
                    /
                    if (b1 == 2) {
                        /
                        gen_op_ld_T0_A0(OT_LONG + s->mem_index);
                        tcg_gen_st32_tl(cpu_T[0], cpu_env, offsetof(CPUX86State,xmm_t0.XMM_L(0)));
                    } else {
                        /
                        gen_ldq_env_A0(s->mem_index, offsetof(CPUX86State,xmm_t0.XMM_D(0)));
                    }
                } else {
                    gen_ldo_env_A0(s->mem_index, op2_offset);
                }
            } else {
                rm = (modrm & 7) | REX_B(s);
                op2_offset = offsetof(CPUX86State,xmm_regs[rm]);
            }
        } else {
            op1_offset = offsetof(CPUX86State,fpregs[reg].mmx);
            if (mod != 3) {
                gen_lea_modrm(s, modrm, &reg_addr, &offset_addr);
                op2_offset = offsetof(CPUX86State,mmx_t0);
                gen_ldq_env_A0(s->mem_index, op2_offset);
            } else {
                rm = (modrm & 7);
                op2_offset = offsetof(CPUX86State,fpregs[rm].mmx);
            }
        }
        switch(b) {
        case 0x0f: /
            if (!(s->cpuid_ext2_features & CPUID_EXT2_3DNOW))
                goto illegal_op;
            val = ldub_code(s->pc++);
            sse_fn_pp = sse_op_table5[val];
            if (!sse_fn_pp) {
                goto illegal_op;
            }
            tcg_gen_addi_ptr(cpu_ptr0, cpu_env, op1_offset);
            tcg_gen_addi_ptr(cpu_ptr1, cpu_env, op2_offset);
            sse_fn_pp(cpu_ptr0, cpu_ptr1);
            break;
        case 0x70: /
        case 0xc6: /
            val = ldub_code(s->pc++);
            tcg_gen_addi_ptr(cpu_ptr0, cpu_env, op1_offset);
            tcg_gen_addi_ptr(cpu_ptr1, cpu_env, op2_offset);
            /
            sse_fn_ppi = (SSEFunc_0_ppi)sse_fn_pp;
            sse_fn_ppi(cpu_ptr0, cpu_ptr1, tcg_const_i32(val));
            break;
        case 0xc2:
            /
            val = ldub_code(s->pc++);
            if (val >= 8)
                goto illegal_op;
            sse_fn_pp = sse_op_table4[val][b1];

            tcg_gen_addi_ptr(cpu_ptr0, cpu_env, op1_offset);
            tcg_gen_addi_ptr(cpu_ptr1, cpu_env, op2_offset);
            sse_fn_pp(cpu_ptr0, cpu_ptr1);
            break;
        case 0xf7:
            /
            if (mod != 3)
                goto illegal_op;
#ifdef TARGET_X86_64
            if (s->aflag == 2) {
                gen_op_movq_A0_reg(R_EDI);
            } else
#endif
            {
                gen_op_movl_A0_reg(R_EDI);
                if (s->aflag == 0)
                    gen_op_andl_A0_ffff();
            }
            gen_add_A0_ds_seg(s);

            tcg_gen_addi_ptr(cpu_ptr0, cpu_env, op1_offset);
            tcg_gen_addi_ptr(cpu_ptr1, cpu_env, op2_offset);
            /
            sse_fn_ppt = (SSEFunc_0_ppt)sse_fn_pp;
            sse_fn_ppt(cpu_ptr0, cpu_ptr1, cpu_A0);
            break;
        default:
            tcg_gen_addi_ptr(cpu_ptr0, cpu_env, op1_offset);
            tcg_gen_addi_ptr(cpu_ptr1, cpu_env, op2_offset);
            sse_fn_pp(cpu_ptr0, cpu_ptr1);
            break;
        }
        if (b == 0x2e || b == 0x2f) {
            s->cc_op = CC_OP_EFLAGS;
        }
    }
}
