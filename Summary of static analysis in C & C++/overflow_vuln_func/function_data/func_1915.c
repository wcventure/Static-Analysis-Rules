bool timerlist_expired(QEMUTimerList *timer_list)
{
    int64_t expire_time;

    if (!atomic_read(&timer_list->active_timers)) {
        return false;
    }

    qemu_mutex_lock(&timer_list->active_timers_lock);
    if (!timer_list->active_timers) {
        qemu_mutex_unlock(&timer_list->active_timers_lock);
        return false;
    }
    expire_time = timer_list->active_timers->expire_time;
    qemu_mutex_unlock(&timer_list->active_timers_lock);

    return expire_time < qemu_clock_get_ns(timer_list->clock->type);
}
