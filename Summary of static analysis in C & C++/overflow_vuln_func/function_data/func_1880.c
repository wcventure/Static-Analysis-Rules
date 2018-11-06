static int ide_drive_pio_post_load(void *opaque, int version_id)
{
    IDEState *s = opaque;

    if (s->end_transfer_fn_idx > ARRAY_SIZE(transfer_end_table)) {
        return -EINVAL;
    }
    s->end_transfer_func = transfer_end_table[s->end_transfer_fn_idx];
    s->data_ptr = s->io_buffer + s->cur_io_buffer_offset;
    s->data_end = s->data_ptr + s->cur_io_buffer_len;

    return 0;
}
