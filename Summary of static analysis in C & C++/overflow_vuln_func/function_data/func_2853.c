int main_loop(void *opaque)
{
    struct pollfd ufds[2], *pf, *serial_ufd, *net_ufd, *gdb_ufd;
    int ret, n, timeout;
    uint8_t ch;
    CPUState *env = global_env;

    if (!term_inited) {
        /
        term_inited = 1;
        term_init();
    }

    for(;;) {
        ret = cpu_x86_exec(env);
        if (reset_requested)
            break;
        if (ret == EXCP_DEBUG)
            return EXCP_DEBUG;
        /
        if (ret == EXCP_HLT) 
            timeout = 10;
        else
            timeout = 0;
        /
        serial_ufd = NULL;
        pf = ufds;
        if (!(serial_ports[0].lsr & UART_LSR_DR)) {
            serial_ufd = pf;
            pf->fd = 0;
            pf->events = POLLIN;
            pf++;
        }
        net_ufd = NULL;
        if (net_fd > 0 && ne2000_can_receive(&ne2000_state)) {
            net_ufd = pf;
            pf->fd = net_fd;
            pf->events = POLLIN;
            pf++;
        }
        gdb_ufd = NULL;
        if (gdbstub_fd > 0) {
            gdb_ufd = pf;
            pf->fd = gdbstub_fd;
            pf->events = POLLIN;
            pf++;
        }

        ret = poll(ufds, pf - ufds, timeout);
        if (ret > 0) {
            if (serial_ufd && (serial_ufd->revents & POLLIN)) {
                n = read(0, &ch, 1);
                if (n == 1) {
                    serial_received_byte(&serial_ports[0], ch);
                }
            }
            if (net_ufd && (net_ufd->revents & POLLIN)) {
                uint8_t buf[MAX_ETH_FRAME_SIZE];

                n = read(net_fd, buf, MAX_ETH_FRAME_SIZE);
                if (n > 0) {
                    if (n < 60) {
                        memset(buf + n, 0, 60 - n);
                        n = 60;
                    }
                    ne2000_receive(&ne2000_state, buf, n);
                }
            }
            if (gdb_ufd && (gdb_ufd->revents & POLLIN)) {
                uint8_t buf[1];
                /
                n = read(gdbstub_fd, buf, 1);
                if (n == 1)
                    break;
            }
        }

        /
        if (timer_irq_pending) {
            pic_set_irq(0, 1);
            pic_set_irq(0, 0);
            timer_irq_pending = 0;
        }

        /
        if (gui_refresh_pending) {
            display_state.dpy_refresh(&display_state);
            gui_refresh_pending = 0;
        }
    }
    return EXCP_INTERRUPT;
}
