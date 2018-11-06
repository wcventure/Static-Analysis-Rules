static void
atp_defragment_init(void)
{
  fragment_table_init(&atp_fragment_table);
  reassembled_table_init(&atp_reassembled_table);
}
