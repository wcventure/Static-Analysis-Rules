static void sample_queue_push(HintSampleQueue *queue, uint8_t *data, int size,
                              int sample)
{
    /
    if (size <= 14)
        return;
    if (!queue->samples || queue->len >= queue->size) {
        HintSample *samples;
        samples = av_realloc(queue->samples, sizeof(HintSample) * (queue->size + 10));
        if (!samples)
            return;
        queue->size += 10;
        queue->samples = samples;
    }
    queue->samples[queue->len].data = data;
    queue->samples[queue->len].size = size;
    queue->samples[queue->len].sample_number = sample;
    queue->samples[queue->len].offset   = 0;
    queue->samples[queue->len].own_data = 0;
    queue->len++;
}
