static void
init_disasm (struct disassemble_info *info)
{
  const struct s390_opcode *opcode;
  const struct s390_opcode *opcode_end;

  memset (opc_index, 0, sizeof (opc_index));
  opcode_end = s390_opcodes + s390_num_opcodes;
  for (opcode = s390_opcodes; opcode < opcode_end; opcode++)
    {
      opc_index[(int) opcode->opcode[0]] = opcode - s390_opcodes;
      while ((opcode < opcode_end) &&
	     (opcode[1].opcode[0] == opcode->opcode[0]))
	opcode++;
    }

#ifdef QEMU_DISABLE
  switch (info->mach)
    {
    case bfd_mach_s390_31:
      current_arch_mask = 1 << S390_OPCODE_ESA;
      break;
    case bfd_mach_s390_64:
      current_arch_mask = 1 << S390_OPCODE_ZARCH;
      break;
    default:
      abort ();
    }
#endif /

  init_flag = 1;
}
