static int configure_filtergraph(FilterGraph *fg)
{
    return fg->graph_desc ? configure_complex_filter(fg) : configure_video_filters(fg);
}
