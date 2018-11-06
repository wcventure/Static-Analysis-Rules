void
unmark_frame(capture_file *cf, frame_data *frame)
{
  frame->flags.marked = FALSE;
  cf->marked_count--;
}
