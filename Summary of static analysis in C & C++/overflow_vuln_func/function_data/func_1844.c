void
mark_frame(capture_file *cf, frame_data *frame)
{
  frame->flags.marked = TRUE;
  cf->marked_count++;
}
