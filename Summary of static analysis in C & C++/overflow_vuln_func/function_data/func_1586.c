static void dissect_r3_upstreammfgfield_dumpm41t81 (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo, proto_tree *tree)
{
  proto_item *rtc_item = proto_tree_add_text (tree, tvb, start_offset, -1, "M41T81 RTC Registers");
  proto_tree *rtc_tree = proto_item_add_subtree (rtc_item, ett_r3m41t81registers);
  proto_tree *tmp_tree [20];
  guint offset_in_bits = start_offset * 8;

  if (tvb_length_remaining (tvb, start_offset) != 20)
    expert_add_info_format (pinfo, tree, PI_UNDECODED, PI_WARN, "Length of M41T81 RTC register dump not 20 octets");
  else
  {
    tmp_tree [ 0] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg00, tvb, start_offset +  0, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [ 1] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg01, tvb, start_offset +  1, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [ 2] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg02, tvb, start_offset +  2, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [ 3] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg03, tvb, start_offset +  3, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [ 4] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg04, tvb, start_offset +  4, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [ 5] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg05, tvb, start_offset +  5, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [ 6] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg06, tvb, start_offset +  6, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [ 7] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg07, tvb, start_offset +  7, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [ 8] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg08, tvb, start_offset +  8, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [ 9] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg09, tvb, start_offset +  9, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [10] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg0a, tvb, start_offset + 10, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [11] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg0b, tvb, start_offset + 11, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [12] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg0c, tvb, start_offset + 12, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [13] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg0d, tvb, start_offset + 13, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [14] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg0e, tvb, start_offset + 14, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [15] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg0f, tvb, start_offset + 15, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [16] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg10, tvb, start_offset + 16, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [17] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg11, tvb, start_offset + 17, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [18] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg12, tvb, start_offset + 18, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);
    tmp_tree [19] = proto_item_add_subtree (proto_tree_add_item (rtc_tree, hf_r3_dumpm41t81_reg13, tvb, start_offset + 19, 1, ENC_LITTLE_ENDIAN), ett_r3m41t81register);

    proto_tree_add_bits_item (tmp_tree [ 0], hf_r3_dumpm41t81_reg00_sec1,       tvb, offset_in_bits, 4, ENC_LITTLE_ENDIAN);  offset_in_bits += 4;
    proto_tree_add_bits_item (tmp_tree [ 0], hf_r3_dumpm41t81_reg00_sec01,      tvb, offset_in_bits, 4, ENC_LITTLE_ENDIAN);  offset_in_bits += 4;
    proto_tree_add_bits_item (tmp_tree [ 1], hf_r3_dumpm41t81_reg01_st,         tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [ 1], hf_r3_dumpm41t81_reg01_10sec,      tvb, offset_in_bits, 3, ENC_LITTLE_ENDIAN);  offset_in_bits += 3;
    proto_tree_add_bits_item (tmp_tree [ 1], hf_r3_dumpm41t81_reg01_1sec,       tvb, offset_in_bits, 4, ENC_LITTLE_ENDIAN);  offset_in_bits += 4;
    proto_tree_add_bits_item (tmp_tree [ 2], hf_r3_dumpm41t81_reg02_notused,    tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [ 2], hf_r3_dumpm41t81_reg02_10min,      tvb, offset_in_bits, 3, ENC_LITTLE_ENDIAN);  offset_in_bits += 3;
    proto_tree_add_bits_item (tmp_tree [ 2], hf_r3_dumpm41t81_reg02_1min,       tvb, offset_in_bits, 4, ENC_LITTLE_ENDIAN);  offset_in_bits += 4;
    proto_tree_add_bits_item (tmp_tree [ 3], hf_r3_dumpm41t81_reg03_cbe,        tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [ 3], hf_r3_dumpm41t81_reg03_cb,         tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [ 3], hf_r3_dumpm41t81_reg03_10hour,     tvb, offset_in_bits, 2, ENC_LITTLE_ENDIAN);  offset_in_bits += 2;
    proto_tree_add_bits_item (tmp_tree [ 3], hf_r3_dumpm41t81_reg03_1hour,      tvb, offset_in_bits, 4, ENC_LITTLE_ENDIAN);  offset_in_bits += 4;
    proto_tree_add_bits_item (tmp_tree [ 4], hf_r3_dumpm41t81_reg04_notused,    tvb, offset_in_bits, 5, ENC_LITTLE_ENDIAN);  offset_in_bits += 5;
    proto_tree_add_bits_item (tmp_tree [ 4], hf_r3_dumpm41t81_reg04_dow,        tvb, offset_in_bits, 3, ENC_LITTLE_ENDIAN);  offset_in_bits += 3;
    proto_tree_add_bits_item (tmp_tree [ 5], hf_r3_dumpm41t81_reg05_notused,    tvb, offset_in_bits, 2, ENC_LITTLE_ENDIAN);  offset_in_bits += 2;
    proto_tree_add_bits_item (tmp_tree [ 5], hf_r3_dumpm41t81_reg05_10day,      tvb, offset_in_bits, 2, ENC_LITTLE_ENDIAN);  offset_in_bits += 2;
    proto_tree_add_bits_item (tmp_tree [ 5], hf_r3_dumpm41t81_reg05_1day,       tvb, offset_in_bits, 4, ENC_LITTLE_ENDIAN);  offset_in_bits += 4;
    proto_tree_add_bits_item (tmp_tree [ 6], hf_r3_dumpm41t81_reg06_notused,    tvb, offset_in_bits, 3, ENC_LITTLE_ENDIAN);  offset_in_bits += 3;
    proto_tree_add_bits_item (tmp_tree [ 6], hf_r3_dumpm41t81_reg06_10month,    tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [ 6], hf_r3_dumpm41t81_reg06_1month,     tvb, offset_in_bits, 4, ENC_LITTLE_ENDIAN);  offset_in_bits += 4;
    proto_tree_add_bits_item (tmp_tree [ 7], hf_r3_dumpm41t81_reg07_10year,     tvb, offset_in_bits, 4, ENC_LITTLE_ENDIAN);  offset_in_bits += 4;
    proto_tree_add_bits_item (tmp_tree [ 7], hf_r3_dumpm41t81_reg07_1year,      tvb, offset_in_bits, 4, ENC_LITTLE_ENDIAN);  offset_in_bits += 4;
    proto_tree_add_bits_item (tmp_tree [ 8], hf_r3_dumpm41t81_reg08_out,        tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [ 8], hf_r3_dumpm41t81_reg08_ft,         tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [ 8], hf_r3_dumpm41t81_reg08_s,          tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [ 8], hf_r3_dumpm41t81_reg08_cal,        tvb, offset_in_bits, 5, ENC_LITTLE_ENDIAN);  offset_in_bits += 5;
    proto_tree_add_bits_item (tmp_tree [ 9], hf_r3_dumpm41t81_reg09_notused,    tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [ 9], hf_r3_dumpm41t81_reg09_bmb,        tvb, offset_in_bits, 5, ENC_LITTLE_ENDIAN);  offset_in_bits += 5;
    proto_tree_add_bits_item (tmp_tree [ 9], hf_r3_dumpm41t81_reg09_rb,         tvb, offset_in_bits, 2, ENC_LITTLE_ENDIAN);  offset_in_bits += 2;
    proto_tree_add_bits_item (tmp_tree [10], hf_r3_dumpm41t81_reg0a_afe,        tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [10], hf_r3_dumpm41t81_reg0a_sqwe,       tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [10], hf_r3_dumpm41t81_reg0a_abe,        tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [10], hf_r3_dumpm41t81_reg0a_10monthalm, tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [10], hf_r3_dumpm41t81_reg0a_1monthalm,  tvb, offset_in_bits, 4, ENC_LITTLE_ENDIAN);  offset_in_bits += 4;
    proto_tree_add_bits_item (tmp_tree [11], hf_r3_dumpm41t81_reg0b_rpt5,       tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [11], hf_r3_dumpm41t81_reg0b_rpt4,       tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [11], hf_r3_dumpm41t81_reg0b_10dayalm,   tvb, offset_in_bits, 2, ENC_LITTLE_ENDIAN);  offset_in_bits += 2;
    proto_tree_add_bits_item (tmp_tree [11], hf_r3_dumpm41t81_reg0b_1dayalm,    tvb, offset_in_bits, 4, ENC_LITTLE_ENDIAN);  offset_in_bits += 4;
    proto_tree_add_bits_item (tmp_tree [12], hf_r3_dumpm41t81_reg0c_rpt3,       tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [12], hf_r3_dumpm41t81_reg0c_ht,         tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [12], hf_r3_dumpm41t81_reg0c_10houralm,  tvb, offset_in_bits, 2, ENC_LITTLE_ENDIAN);  offset_in_bits += 2;
    proto_tree_add_bits_item (tmp_tree [12], hf_r3_dumpm41t81_reg0c_1houralm,   tvb, offset_in_bits, 4, ENC_LITTLE_ENDIAN);  offset_in_bits += 4;
    proto_tree_add_bits_item (tmp_tree [13], hf_r3_dumpm41t81_reg0d_rpt2,       tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [13], hf_r3_dumpm41t81_reg0d_10minalm,   tvb, offset_in_bits, 3, ENC_LITTLE_ENDIAN);  offset_in_bits += 3;
    proto_tree_add_bits_item (tmp_tree [13], hf_r3_dumpm41t81_reg0d_1minalm,    tvb, offset_in_bits, 4, ENC_LITTLE_ENDIAN);  offset_in_bits += 4;
    proto_tree_add_bits_item (tmp_tree [14], hf_r3_dumpm41t81_reg0e_rpt1,       tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [14], hf_r3_dumpm41t81_reg0e_10secalm,   tvb, offset_in_bits, 3, ENC_LITTLE_ENDIAN);  offset_in_bits += 3;
    proto_tree_add_bits_item (tmp_tree [14], hf_r3_dumpm41t81_reg0e_1secalm,    tvb, offset_in_bits, 4, ENC_LITTLE_ENDIAN);  offset_in_bits += 4;
    proto_tree_add_bits_item (tmp_tree [15], hf_r3_dumpm41t81_reg0f_wdf,        tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [15], hf_r3_dumpm41t81_reg0f_af,         tvb, offset_in_bits, 1, ENC_LITTLE_ENDIAN);  offset_in_bits += 1;
    proto_tree_add_bits_item (tmp_tree [15], hf_r3_dumpm41t81_reg0f_notused,    tvb, offset_in_bits, 6, ENC_LITTLE_ENDIAN);  offset_in_bits += 6;
    proto_tree_add_bits_item (tmp_tree [16], hf_r3_dumpm41t81_reg10_notused,    tvb, offset_in_bits, 8, ENC_LITTLE_ENDIAN);  offset_in_bits += 8;
    proto_tree_add_bits_item (tmp_tree [17], hf_r3_dumpm41t81_reg11_notused,    tvb, offset_in_bits, 8, ENC_LITTLE_ENDIAN);  offset_in_bits += 8;
    proto_tree_add_bits_item (tmp_tree [18], hf_r3_dumpm41t81_reg12_notused,    tvb, offset_in_bits, 8, ENC_LITTLE_ENDIAN);  offset_in_bits += 8;
    proto_tree_add_bits_item (tmp_tree [19], hf_r3_dumpm41t81_reg13_rs,         tvb, offset_in_bits - 8, 4, ENC_LITTLE_ENDIAN);  offset_in_bits += 4;
    proto_tree_add_bits_item (tmp_tree [19], hf_r3_dumpm41t81_reg13_notused,    tvb, offset_in_bits - 8, 4, ENC_LITTLE_ENDIAN);
  }
}
