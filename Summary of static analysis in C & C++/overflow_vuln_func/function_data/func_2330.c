void do_tw (int flags)
{
    if (!likely(!((Ts0 < Ts1 && (flags & 0x10)) ||
                  (Ts0 > Ts1 && (flags & 0x08)) ||
                  (Ts0 == Ts1 && (flags & 0x04)) ||
                  (T0 < T1 && (flags & 0x02)) ||
                  (T0 > T1 && (flags & 0x01)))))
        do_raise_exception_err(EXCP_PROGRAM, EXCP_TRAP);
}
