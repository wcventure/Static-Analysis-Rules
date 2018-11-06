static void
dissect_homeplug_av_nw_info_sta(ptvcursor_t *cursor, gboolean vendor, guint homeplug_av_mmver)
{
   proto_item *it;

   if (!ptvcursor_tree(cursor))
      return;

   it = ptvcursor_add_no_advance(cursor, hf_homeplug_av_nw_info_sta_info, -1, ENC_NA);

   ptvcursor_push_subtree(cursor, it, ett_homeplug_av_nw_info_sta_info);
   {
      ptvcursor_add(cursor, hf_homeplug_av_nw_info_sta_da, 6, ENC_NA);
      if (vendor) {
         ptvcursor_add(cursor, hf_homeplug_av_nw_info_sta_tei, 1, ENC_BIG_ENDIAN);

         if (homeplug_av_mmver == 1)
            ptvcursor_add(cursor, hf_homeplug_av_reserved, 3, ENC_NA);

         ptvcursor_add(cursor, hf_homeplug_av_nw_info_sta_bda, 6, ENC_NA);
      }
      if (!homeplug_av_mmver)
      {
         ptvcursor_add(cursor, hf_homeplug_av10_nw_info_sta_phy_dr_tx, 1, ENC_BIG_ENDIAN);
         ptvcursor_add(cursor, hf_homeplug_av10_nw_info_sta_phy_dr_rx, 1, ENC_BIG_ENDIAN);
      }
      else if (homeplug_av_mmver == 1)
      {
         ptvcursor_add(cursor, hf_homeplug_av11_nw_info_sta_phy_dr_tx, 2, ENC_LITTLE_ENDIAN);
         ptvcursor_add_no_advance(cursor, hf_homeplug_av11_nw_info_sta_cpling_tx, 1, ENC_BIG_ENDIAN);
         ptvcursor_add(cursor, hf_homeplug_av11_nw_info_sta_cpling_rx, 1, ENC_BIG_ENDIAN);
         ptvcursor_add(cursor, hf_homeplug_av_reserved, 1, ENC_NA);
         ptvcursor_add(cursor, hf_homeplug_av11_nw_info_sta_phy_dr_rx, 2, ENC_LITTLE_ENDIAN);
         ptvcursor_add(cursor, hf_homeplug_av_reserved, 1, ENC_NA);
      }
   }
   ptvcursor_pop_subtree(cursor);
}
