static void
e1000e_set_icr(E1000ECore *core, int index, uint32_t val)
{
    if ((core->mac[ICR] & E1000_ICR_ASSERTED) &&
        (core->mac[CTRL_EXT] & E1000_CTRL_EXT_IAME)) {
        trace_e1000e_irq_icr_process_iame();
        e1000e_clear_ims_bits(core, core->mac[IAM]);
    }

    trace_e1000e_irq_icr_write(val, core->mac[ICR], core->mac[ICR] & ~val);
    core->mac[ICR] &= ~val;
    e1000e_update_interrupt_state(core);
}
