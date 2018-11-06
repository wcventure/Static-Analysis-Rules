static inline int vmsvga_fifo_empty(struct vmsvga_state_s *s)
{
    if (!s->config || !s->enable)
        return 1;
    return (s->cmd->next_cmd == s->cmd->stop);
}
