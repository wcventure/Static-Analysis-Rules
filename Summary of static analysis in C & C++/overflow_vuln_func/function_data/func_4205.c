static void end_frame(AVFilterLink *inlink)
{
    avfilter_end_frame(inlink->dst->outputs[0]);
    avfilter_unref_buffer(inlink->cur_buf);
}
