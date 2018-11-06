static void ehci_frame_timer(void *opaque)
{
    EHCIState *ehci = opaque;
    int need_timer = 0;
    int64_t expire_time, t_now;
    uint64_t ns_elapsed;
    int uframes, skipped_uframes;
    int i;

    t_now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    ns_elapsed = t_now - ehci->last_run_ns;
    uframes = ns_elapsed / UFRAME_TIMER_NS;

    if (ehci_periodic_enabled(ehci) || ehci->pstate != EST_INACTIVE) {
        need_timer++;

        if (uframes > (ehci->maxframes * 8)) {
            skipped_uframes = uframes - (ehci->maxframes * 8);
            ehci_update_frindex(ehci, skipped_uframes);
            ehci->last_run_ns += UFRAME_TIMER_NS * skipped_uframes;
            uframes -= skipped_uframes;
            DPRINTF("WARNING - EHCI skipped %d uframes\n", skipped_uframes);
        }

        for (i = 0; i < uframes; i++) {
            /
            if (i >= MIN_UFR_PER_TICK) {
                ehci_commit_irq(ehci);
                if ((ehci->usbsts & USBINTR_MASK) & ehci->usbintr) {
                    break;
                }
            }
            if (ehci->periodic_sched_active) {
                ehci->periodic_sched_active--;
            }
            ehci_update_frindex(ehci, 1);
            if ((ehci->frindex & 7) == 0) {
                ehci_advance_periodic_state(ehci);
            }
            ehci->last_run_ns += UFRAME_TIMER_NS;
        }
    } else {
        ehci->periodic_sched_active = 0;
        ehci_update_frindex(ehci, uframes);
        ehci->last_run_ns += UFRAME_TIMER_NS * uframes;
    }

    if (ehci->periodic_sched_active) {
        ehci->async_stepdown = 0;
    } else if (ehci->async_stepdown < ehci->maxframes / 2) {
        ehci->async_stepdown++;
    }

    /
    if (ehci_async_enabled(ehci) || ehci->astate != EST_INACTIVE) {
        need_timer++;
        ehci_advance_async_state(ehci);
    }

    ehci_commit_irq(ehci);
    if (ehci->usbsts_pending) {
        need_timer++;
        ehci->async_stepdown = 0;
    }

    if (ehci_enabled(ehci) && (ehci->usbintr & USBSTS_FLR)) {
        need_timer++;
    }

    if (need_timer) {
        /
        if (ehci->int_req_by_async && (ehci->usbsts & USBSTS_INT)) {
            expire_time = t_now +
                NANOSECONDS_PER_SECOND / (FRAME_TIMER_FREQ * 4);
            ehci->int_req_by_async = false;
        } else {
            expire_time = t_now + (NANOSECONDS_PER_SECOND
                               * (ehci->async_stepdown+1) / FRAME_TIMER_FREQ);
        }
        timer_mod(ehci->frame_timer, expire_time);
    }
}
