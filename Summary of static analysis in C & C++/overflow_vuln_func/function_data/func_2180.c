void register_m68k_insns (CPUM68KState *env)
{
    /
    if (opcode_table[0] != NULL) {
        return;
    }

    /
#define BASE(name, opcode, mask) \
    register_opcode(disas_##name, 0x##opcode, 0x##mask)
#define INSN(name, opcode, mask, feature) do { \
    if (m68k_feature(env, M68K_FEATURE_##feature)) \
        BASE(name, opcode, mask); \
    } while(0)
    BASE(undef,     0000, 0000);
    INSN(arith_im,  0080, fff8, CF_ISA_A);
    INSN(arith_im,  0000, ff00, M68000);
    INSN(undef,     00c0, ffc0, M68000);
    INSN(bitrev,    00c0, fff8, CF_ISA_APLUSC);
    BASE(bitop_reg, 0100, f1c0);
    BASE(bitop_reg, 0140, f1c0);
    BASE(bitop_reg, 0180, f1c0);
    BASE(bitop_reg, 01c0, f1c0);
    INSN(arith_im,  0280, fff8, CF_ISA_A);
    INSN(arith_im,  0200, ff00, M68000);
    INSN(undef,     02c0, ffc0, M68000);
    INSN(byterev,   02c0, fff8, CF_ISA_APLUSC);
    INSN(arith_im,  0480, fff8, CF_ISA_A);
    INSN(arith_im,  0400, ff00, M68000);
    INSN(undef,     04c0, ffc0, M68000);
    INSN(arith_im,  0600, ff00, M68000);
    INSN(undef,     06c0, ffc0, M68000);
    INSN(ff1,       04c0, fff8, CF_ISA_APLUSC);
    INSN(arith_im,  0680, fff8, CF_ISA_A);
    INSN(arith_im,  0c00, ff38, CF_ISA_A);
    INSN(arith_im,  0c00, ff00, M68000);
    BASE(bitop_im,  0800, ffc0);
    BASE(bitop_im,  0840, ffc0);
    BASE(bitop_im,  0880, ffc0);
    BASE(bitop_im,  08c0, ffc0);
    INSN(arith_im,  0a80, fff8, CF_ISA_A);
    INSN(arith_im,  0a00, ff00, M68000);
    INSN(cas,       0ac0, ffc0, CAS);
    INSN(cas,       0cc0, ffc0, CAS);
    INSN(cas,       0ec0, ffc0, CAS);
    INSN(cas2w,     0cfc, ffff, CAS);
    INSN(cas2l,     0efc, ffff, CAS);
    BASE(move,      1000, f000);
    BASE(move,      2000, f000);
    BASE(move,      3000, f000);
    INSN(strldsr,   40e7, ffff, CF_ISA_APLUSC);
    INSN(negx,      4080, fff8, CF_ISA_A);
    INSN(negx,      4000, ff00, M68000);
    INSN(undef,     40c0, ffc0, M68000);
    INSN(move_from_sr, 40c0, fff8, CF_ISA_A);
    INSN(move_from_sr, 40c0, ffc0, M68000);
    BASE(lea,       41c0, f1c0);
    BASE(clr,       4200, ff00);
    BASE(undef,     42c0, ffc0);
    INSN(move_from_ccr, 42c0, fff8, CF_ISA_A);
    INSN(move_from_ccr, 42c0, ffc0, M68000);
    INSN(neg,       4480, fff8, CF_ISA_A);
    INSN(neg,       4400, ff00, M68000);
    INSN(undef,     44c0, ffc0, M68000);
    BASE(move_to_ccr, 44c0, ffc0);
    INSN(not,       4680, fff8, CF_ISA_A);
    INSN(not,       4600, ff00, M68000);
    INSN(undef,     46c0, ffc0, M68000);
    INSN(move_to_sr, 46c0, ffc0, CF_ISA_A);
    INSN(nbcd,      4800, ffc0, M68000);
    INSN(linkl,     4808, fff8, M68000);
    BASE(pea,       4840, ffc0);
    BASE(swap,      4840, fff8);
    INSN(bkpt,      4848, fff8, BKPT);
    INSN(movem,     48d0, fbf8, CF_ISA_A);
    INSN(movem,     48e8, fbf8, CF_ISA_A);
    INSN(movem,     4880, fb80, M68000);
    BASE(ext,       4880, fff8);
    BASE(ext,       48c0, fff8);
    BASE(ext,       49c0, fff8);
    BASE(tst,       4a00, ff00);
    INSN(tas,       4ac0, ffc0, CF_ISA_B);
    INSN(tas,       4ac0, ffc0, M68000);
    INSN(halt,      4ac8, ffff, CF_ISA_A);
    INSN(pulse,     4acc, ffff, CF_ISA_A);
    BASE(illegal,   4afc, ffff);
    INSN(mull,      4c00, ffc0, CF_ISA_A);
    INSN(mull,      4c00, ffc0, LONG_MULDIV);
    INSN(divl,      4c40, ffc0, CF_ISA_A);
    INSN(divl,      4c40, ffc0, LONG_MULDIV);
    INSN(sats,      4c80, fff8, CF_ISA_B);
    BASE(trap,      4e40, fff0);
    BASE(link,      4e50, fff8);
    BASE(unlk,      4e58, fff8);
    INSN(move_to_usp, 4e60, fff8, USP);
    INSN(move_from_usp, 4e68, fff8, USP);
    BASE(nop,       4e71, ffff);
    BASE(stop,      4e72, ffff);
    BASE(rte,       4e73, ffff);
    INSN(rtd,       4e74, ffff, RTD);
    BASE(rts,       4e75, ffff);
    INSN(movec,     4e7b, ffff, CF_ISA_A);
    BASE(jump,      4e80, ffc0);
    BASE(jump,      4ec0, ffc0);
    INSN(addsubq,   5000, f080, M68000);
    BASE(addsubq,   5080, f0c0);
    INSN(scc,       50c0, f0f8, CF_ISA_A); /
    INSN(scc,       50c0, f0c0, M68000);   /
    INSN(dbcc,      50c8, f0f8, M68000);
    INSN(tpf,       51f8, fff8, CF_ISA_A);

    /
    BASE(branch,    6000, f000);
    /
    BASE(undef,     60ff, f0ff); /
    INSN(branch,    60ff, f0ff, CF_ISA_B);
    INSN(undef,     60ff, ffff, CF_ISA_B); /
    INSN(branch,    60ff, ffff, BRAL);
    INSN(branch,    60ff, f0ff, BCCL);

    BASE(moveq,     7000, f100);
    INSN(mvzs,      7100, f100, CF_ISA_B);
    BASE(or,        8000, f000);
    BASE(divw,      80c0, f0c0);
    INSN(sbcd_reg,  8100, f1f8, M68000);
    INSN(sbcd_mem,  8108, f1f8, M68000);
    BASE(addsub,    9000, f000);
    INSN(undef,     90c0, f0c0, CF_ISA_A);
    INSN(subx_reg,  9180, f1f8, CF_ISA_A);
    INSN(subx_reg,  9100, f138, M68000);
    INSN(subx_mem,  9108, f138, M68000);
    INSN(suba,      91c0, f1c0, CF_ISA_A);
    INSN(suba,      90c0, f0c0, M68000);

    BASE(undef_mac, a000, f000);
    INSN(mac,       a000, f100, CF_EMAC);
    INSN(from_mac,  a180, f9b0, CF_EMAC);
    INSN(move_mac,  a110, f9fc, CF_EMAC);
    INSN(from_macsr,a980, f9f0, CF_EMAC);
    INSN(from_mask, ad80, fff0, CF_EMAC);
    INSN(from_mext, ab80, fbf0, CF_EMAC);
    INSN(macsr_to_ccr, a9c0, ffff, CF_EMAC);
    INSN(to_mac,    a100, f9c0, CF_EMAC);
    INSN(to_macsr,  a900, ffc0, CF_EMAC);
    INSN(to_mext,   ab00, fbc0, CF_EMAC);
    INSN(to_mask,   ad00, ffc0, CF_EMAC);

    INSN(mov3q,     a140, f1c0, CF_ISA_B);
    INSN(cmp,       b000, f1c0, CF_ISA_B); /
    INSN(cmp,       b040, f1c0, CF_ISA_B); /
    INSN(cmpa,      b0c0, f1c0, CF_ISA_B); /
    INSN(cmp,       b080, f1c0, CF_ISA_A);
    INSN(cmpa,      b1c0, f1c0, CF_ISA_A);
    INSN(cmp,       b000, f100, M68000);
    INSN(eor,       b100, f100, M68000);
    INSN(cmpm,      b108, f138, M68000);
    INSN(cmpa,      b0c0, f0c0, M68000);
    INSN(eor,       b180, f1c0, CF_ISA_A);
    BASE(and,       c000, f000);
    INSN(exg_dd,    c140, f1f8, M68000);
    INSN(exg_aa,    c148, f1f8, M68000);
    INSN(exg_da,    c188, f1f8, M68000);
    BASE(mulw,      c0c0, f0c0);
    INSN(abcd_reg,  c100, f1f8, M68000);
    INSN(abcd_mem,  c108, f1f8, M68000);
    BASE(addsub,    d000, f000);
    INSN(undef,     d0c0, f0c0, CF_ISA_A);
    INSN(addx_reg,      d180, f1f8, CF_ISA_A);
    INSN(addx_reg,  d100, f138, M68000);
    INSN(addx_mem,  d108, f138, M68000);
    INSN(adda,      d1c0, f1c0, CF_ISA_A);
    INSN(adda,      d0c0, f0c0, M68000);
    INSN(shift_im,  e080, f0f0, CF_ISA_A);
    INSN(shift_reg, e0a0, f0f0, CF_ISA_A);
    INSN(shift8_im, e000, f0f0, M68000);
    INSN(shift16_im, e040, f0f0, M68000);
    INSN(shift_im,  e080, f0f0, M68000);
    INSN(shift8_reg, e020, f0f0, M68000);
    INSN(shift16_reg, e060, f0f0, M68000);
    INSN(shift_reg, e0a0, f0f0, M68000);
    INSN(shift_mem, e0c0, fcc0, M68000);
    INSN(rotate_im, e090, f0f0, M68000);
    INSN(rotate8_im, e010, f0f0, M68000);
    INSN(rotate16_im, e050, f0f0, M68000);
    INSN(rotate_reg, e0b0, f0f0, M68000);
    INSN(rotate8_reg, e030, f0f0, M68000);
    INSN(rotate16_reg, e070, f0f0, M68000);
    INSN(rotate_mem, e4c0, fcc0, M68000);
    INSN(bfext_mem, e9c0, fdc0, BITFIELD);  /
    INSN(bfext_reg, e9c0, fdf8, BITFIELD);
    INSN(bfins_mem, efc0, ffc0, BITFIELD);
    INSN(bfins_reg, efc0, fff8, BITFIELD);
    INSN(bfop_mem, eac0, ffc0, BITFIELD);   /
    INSN(bfop_reg, eac0, fff8, BITFIELD);   /
    INSN(bfop_mem, ecc0, ffc0, BITFIELD);   /
    INSN(bfop_reg, ecc0, fff8, BITFIELD);   /
    INSN(bfop_mem, edc0, ffc0, BITFIELD);   /
    INSN(bfop_reg, edc0, fff8, BITFIELD);   /
    INSN(bfop_mem, eec0, ffc0, BITFIELD);   /
    INSN(bfop_reg, eec0, fff8, BITFIELD);   /
    INSN(bfop_mem, e8c0, ffc0, BITFIELD);   /
    INSN(bfop_reg, e8c0, fff8, BITFIELD);   /
    BASE(undef_fpu, f000, f000);
    INSN(fpu,       f200, ffc0, CF_FPU);
    INSN(fbcc,      f280, ffc0, CF_FPU);
    INSN(frestore,  f340, ffc0, CF_FPU);
    INSN(fsave,     f300, ffc0, CF_FPU);
    INSN(fpu,       f200, ffc0, FPU);
    INSN(fscc,      f240, ffc0, FPU);
    INSN(fbcc,      f280, ff80, FPU);
    INSN(frestore,  f340, ffc0, FPU);
    INSN(fsave,     f300, ffc0, FPU);
    INSN(intouch,   f340, ffc0, CF_ISA_A);
    INSN(cpushl,    f428, ff38, CF_ISA_A);
    INSN(wddata,    fb00, ff00, CF_ISA_A);
    INSN(wdebug,    fbc0, ffc0, CF_ISA_A);
#undef INSN
}
