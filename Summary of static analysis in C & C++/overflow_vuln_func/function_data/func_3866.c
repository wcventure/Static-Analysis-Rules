#if HAVE_PTHREADS
static void *circular_buffer_task( void *_URLContext)
{
    URLContext *h = _URLContext;
    UDPContext *s = h->priv_data;
    fd_set rfds;
    struct timeval tv;

    while(!s->exit_thread) {
        int left;
        int ret;
        int len;

        if (ff_check_interrupt(&h->interrupt_callback)) {
            s->circular_buffer_error = AVERROR(EINTR);
            goto end;
        }

        FD_ZERO(&rfds);
        FD_SET(s->udp_fd, &rfds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        ret = select(s->udp_fd + 1, &rfds, NULL, NULL, &tv);
        if (ret < 0) {
            if (ff_neterrno() == AVERROR(EINTR))
                continue;
            s->circular_buffer_error = AVERROR(EIO);
            goto end;
        }

        if (!(ret > 0 && FD_ISSET(s->udp_fd, &rfds)))
            continue;

        /
        /
        left = av_fifo_space(s->fifo);

        /
        if(left < UDP_MAX_PKT_SIZE + 4) {
            av_log(h, AV_LOG_ERROR, "circular_buffer: OVERRUN\n");
            s->circular_buffer_error = AVERROR(EIO);
            goto end;
        }
        len = recv(s->udp_fd, s->tmp+4, sizeof(s->tmp)-4, 0);
        if (len < 0) {
            if (ff_neterrno() != AVERROR(EAGAIN) && ff_neterrno() != AVERROR(EINTR)) {
                s->circular_buffer_error = AVERROR(EIO);
                goto end;
            }
            continue;
        }
        AV_WL32(s->tmp, len);
        pthread_mutex_lock(&s->mutex);
        av_fifo_generic_write(s->fifo, s->tmp, len+4, NULL);
        pthread_cond_signal(&s->cond);
        pthread_mutex_unlock(&s->mutex);
    }

end:
    pthread_mutex_lock(&s->mutex);
    pthread_cond_signal(&s->cond);
    pthread_mutex_unlock(&s->mutex);
    return NULL;
}
