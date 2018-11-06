static void
dissect_wlan_radio_phdr (tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree, void *data)
{
  struct ieee_802_11_phdr *phdr = (struct ieee_802_11_phdr *)data;
  proto_item *ti = NULL;
  proto_tree *radio_tree = NULL;
  float data_rate = 0.0f;
  gboolean have_data_rate = FALSE;
  gboolean has_short_preamble = FALSE;
  gboolean short_preamble = 1;

  guint frame_length = tvb_reported_length(tvb); /

  /
  guint preamble = 0; /
  guint duration = 0; /

  col_set_str(pinfo->cinfo, COL_PROTOCOL, "Radio");
  col_clear(pinfo->cinfo, COL_INFO);

  /
  if (phdr->has_data_rate) {
    data_rate = phdr->data_rate * 0.5f;
    have_data_rate = TRUE;
  }

  if (phdr->has_signal_dbm) {
    col_add_fstr(pinfo->cinfo, COL_RSSI, "%d dBm", phdr->signal_dbm);
  } else if (phdr->has_signal_percent) {
    col_add_fstr(pinfo->cinfo, COL_RSSI, "%u%%", phdr->signal_percent);
  }

  if (tree) {
    ti = proto_tree_add_item(tree, proto_wlan_radio, tvb, 0, 0, ENC_NA);
    radio_tree = proto_item_add_subtree (ti, ett_wlan_radio);

    if (phdr->phy != PHDR_802_11_PHY_UNKNOWN) {
      proto_tree_add_uint(radio_tree, hf_wlan_radio_phy, tvb, 0, 0,
               phdr->phy);

      switch (phdr->phy) {

      case PHDR_802_11_PHY_11_FHSS:
      {
        struct ieee_802_11_fhss *info_fhss = &phdr->phy_info.info_11_fhss;

        if (info_fhss->has_hop_set) {
          proto_tree_add_uint(radio_tree, hf_wlan_radio_11_fhss_hop_set, tvb, 0, 0,
                   info_fhss->hop_set);
        }
        if (info_fhss->has_hop_pattern) {
          proto_tree_add_uint(radio_tree, hf_wlan_radio_11_fhss_hop_pattern, tvb, 0, 0,
                   info_fhss->hop_pattern);
        }
        if (info_fhss->has_hop_index) {
          proto_tree_add_uint(radio_tree, hf_wlan_radio_11_fhss_hop_index, tvb, 0, 0,
                   info_fhss->hop_index);
        }
        break;
      }

      case PHDR_802_11_PHY_11B:
      {
        struct ieee_802_11b *info_b = &phdr->phy_info.info_11b;

        has_short_preamble = info_b->has_short_preamble;
        short_preamble = info_b->short_preamble;

        if (has_short_preamble) {
          proto_tree_add_boolean(radio_tree, hf_wlan_radio_short_preamble, tvb, 0, 0,
                   short_preamble);
        }
        break;
      }

      case PHDR_802_11_PHY_11A:
      {
        struct ieee_802_11a *info_a = &phdr->phy_info.info_11a;

        if (info_a->has_channel_type) {
          proto_tree_add_uint(radio_tree, hf_wlan_radio_11a_channel_type, tvb, 0, 0,
                   info_a->channel_type);
        }
        if (info_a->has_turbo_type) {
          proto_tree_add_uint(radio_tree, hf_wlan_radio_11a_turbo_type, tvb, 0, 0,
                   info_a->turbo_type);
        }
        break;
      }

      case PHDR_802_11_PHY_11G:
      {
        struct ieee_802_11g *info_g = &phdr->phy_info.info_11g;

        has_short_preamble = info_g->has_short_preamble;
        short_preamble = info_g->short_preamble;

        if (has_short_preamble) {
          proto_tree_add_boolean(radio_tree, hf_wlan_radio_short_preamble, tvb, 0, 0,
                   short_preamble);
        }
        if (info_g->has_mode) {
          proto_tree_add_uint(radio_tree, hf_wlan_radio_11g_mode, tvb, 0, 0,
                   info_g->mode);
        }
        break;
      }

      case PHDR_802_11_PHY_11N:
        {
          struct ieee_802_11n *info_n = &phdr->phy_info.info_11n;
          guint bandwidth_40;

          if (info_n->has_mcs_index) {
            proto_tree_add_uint(radio_tree, hf_wlan_radio_11n_mcs_index, tvb, 0, 0,
                     info_n->mcs_index);
          }

          if (info_n->has_bandwidth) {
            proto_tree_add_uint(radio_tree, hf_wlan_radio_11n_bandwidth, tvb, 0, 0,
                     info_n->bandwidth);
          }

          if (info_n->has_short_gi) {
            proto_tree_add_boolean(radio_tree, hf_wlan_radio_11n_short_gi, tvb, 0, 0,
                     info_n->short_gi);
          }

          if (info_n->has_greenfield) {
            proto_tree_add_boolean(radio_tree, hf_wlan_radio_11n_greenfield, tvb, 0, 0,
                     info_n->greenfield);
          }

          if (info_n->has_fec) {
            proto_tree_add_uint(radio_tree, hf_wlan_radio_11n_fec, tvb, 0, 0,
                     info_n->fec);
          }

          if (info_n->has_stbc_streams) {
            proto_tree_add_uint(radio_tree, hf_wlan_radio_11n_stbc_streams, tvb, 0, 0,
                     info_n->stbc_streams);
          }

          if (info_n->has_ness) {
            proto_tree_add_uint(radio_tree, hf_wlan_radio_11n_ness, tvb, 0, 0,
                     info_n->ness);
          }

          /
          if (info_n->has_mcs_index &&
              info_n->has_bandwidth &&
              info_n->has_short_gi) {
            bandwidth_40 =
              (info_n->bandwidth == PHDR_802_11_BANDWIDTH_40_MHZ) ?
               1 : 0;
            if (info_n->mcs_index <= MAX_MCS_INDEX) {
              data_rate = ieee80211_htrate(info_n->mcs_index, bandwidth_40, info_n->short_gi);
              have_data_rate = TRUE;
            }
          }
        }
        break;

      case PHDR_802_11_PHY_11AC:
        {
          struct ieee_802_11ac *info_ac = &phdr->phy_info.info_11ac;
          gboolean can_calculate_rate;
          guint bandwidth = 0;
          guint i;

          if (info_ac->has_stbc) {
            proto_tree_add_boolean(radio_tree, hf_wlan_radio_11ac_stbc, tvb, 0, 0,
                     info_ac->stbc);
          }

          if (info_ac->has_txop_ps_not_allowed) {
            proto_tree_add_boolean(radio_tree, hf_wlan_radio_11ac_txop_ps_not_allowed, tvb, 0, 0,
                     info_ac->txop_ps_not_allowed);
          }

          if (info_ac->has_short_gi) {
            can_calculate_rate = TRUE;  /
            proto_tree_add_boolean(radio_tree, hf_wlan_radio_11ac_short_gi, tvb, 0, 0,
                     info_ac->short_gi);
          } else {
            can_calculate_rate = FALSE; /
          }

          if (info_ac->has_short_gi_nsym_disambig) {
            proto_tree_add_boolean(radio_tree, hf_wlan_radio_11ac_short_gi_nsym_disambig, tvb, 0, 0,
                     info_ac->short_gi_nsym_disambig);
          }

          if (info_ac->has_ldpc_extra_ofdm_symbol) {
            proto_tree_add_boolean(radio_tree, hf_wlan_radio_11ac_ldpc_extra_ofdm_symbol, tvb, 0, 0,
                     info_ac->ldpc_extra_ofdm_symbol);
          }

          if (info_ac->has_beamformed) {
            proto_tree_add_boolean(radio_tree, hf_wlan_radio_11ac_beamformed, tvb, 0, 0,
                     info_ac->beamformed);
          }

          if (info_ac->has_bandwidth) {
            if (info_ac->bandwidth < G_N_ELEMENTS(ieee80211_vht_bw2rate_index))
              bandwidth = ieee80211_vht_bw2rate_index[info_ac->bandwidth];
            else
              can_calculate_rate = FALSE; /
            proto_tree_add_uint(radio_tree, hf_wlan_radio_11ac_bandwidth, tvb, 0, 0,
                     info_ac->bandwidth);
          } else {
            can_calculate_rate = FALSE;   /
          }

          for (i = 0; i < 4; i++) {

            if (info_ac->nss[i] != 0) {
              proto_item *it;
              proto_tree *user_tree;

              it = proto_tree_add_item(radio_tree, hf_wlan_radio_11ac_user, tvb, 0, 0, ENC_NA);
              proto_item_append_text(it, " %d: MCS %u", i, info_ac->mcs[i]);
              user_tree = proto_item_add_subtree(it, ett_wlan_radio_11ac_user);

              it = proto_tree_add_uint(user_tree, hf_wlan_radio_11ac_mcs, tvb, 0, 0,
                      info_ac->mcs[i]);
              if (info_ac->mcs[i] > MAX_MCS_VHT_INDEX) {
                proto_item_append_text(it, " (invalid)");
              } else {
                proto_item_append_text(it, " (%s %s)",
                  ieee80211_vhtinfo[info_ac->mcs[i]].modulation,
                  ieee80211_vhtinfo[info_ac->mcs[i]].coding_rate);
              }

              proto_tree_add_uint(user_tree, hf_wlan_radio_11ac_nss, tvb, 0, 0,
                       info_ac->nss[i]);
              /
              if (info_ac->has_stbc) {
                guint nsts;

                if (info_ac->stbc)
                  nsts = 2 * info_ac->nss[i];
                else
                  nsts = info_ac->nss[i];
                proto_tree_add_uint(user_tree, hf_wlan_radio_11ac_nsts, tvb, 0, 0,
                       nsts);
              }
              if (info_ac->has_fec) {
                  proto_tree_add_uint(user_tree, hf_wlan_radio_11ac_fec, tvb, 0, 0,
                           (info_ac->fec >> i) & 0x01);
              }

              /
              if (can_calculate_rate && info_ac->mcs[i] <= MAX_MCS_VHT_INDEX &&
                  info_ac->nss[i] <= MAX_VHT_NSS &&
                  ieee80211_vhtvalid[info_ac->mcs[i]].valid[bandwidth][info_ac->nss[i]]) {
                data_rate = ieee80211_vhtrate(info_ac->mcs[i], bandwidth, info_ac->short_gi) * info_ac->nss[i];
                if (data_rate != 0.0f) {
                  proto_tree_add_float_format_value(user_tree, hf_wlan_radio_data_rate, tvb, 0, 0,
                        data_rate,
                        "%.1f Mb/s",
                       data_rate);
                }
              }
            }
          }

          if (info_ac->has_group_id) {
            proto_tree_add_uint(radio_tree, hf_wlan_radio_11ac_gid, tvb, 0, 0,
                     info_ac->group_id);
          }

          if (info_ac->has_partial_aid) {
            proto_tree_add_uint(radio_tree, hf_wlan_radio_11ac_p_aid, tvb, 0, 0,
                     info_ac->partial_aid);
          }
        }
        break;
      }
    }

    if (have_data_rate) {
      col_add_fstr(pinfo->cinfo, COL_TX_RATE, "%.1f", data_rate);
      proto_tree_add_float_format_value(radio_tree, hf_wlan_radio_data_rate, tvb, 0, 0,
               data_rate,
               "%.1f Mb/s",
               data_rate);
    }

    if (phdr->has_channel) {
      col_add_fstr(pinfo->cinfo, COL_FREQ_CHAN, "%u", phdr->channel);
      proto_tree_add_uint(radio_tree, hf_wlan_radio_channel, tvb, 0, 0,
              phdr->channel);
    }

    if (phdr->has_frequency) {
      col_add_fstr(pinfo->cinfo, COL_FREQ_CHAN, "%u MHz", phdr->frequency);
      proto_tree_add_uint_format_value(radio_tree, hf_wlan_radio_frequency, tvb, 0, 0,
              phdr->frequency,
              "%u MHz",
              phdr->frequency);
    }

    if (phdr->has_signal_percent) {
      col_add_fstr(pinfo->cinfo, COL_RSSI, "%u%%", phdr->signal_percent);
      proto_tree_add_uint_format_value(radio_tree, hf_wlan_radio_signal_percent, tvb, 0, 0,
              phdr->signal_percent,
              "%u%%",
              phdr->signal_percent);
    }

    if (phdr->has_signal_dbm) {
      col_add_fstr(pinfo->cinfo, COL_RSSI, "%d dBm", phdr->signal_dbm);
      proto_tree_add_int_format_value(radio_tree, hf_wlan_radio_signal_dbm, tvb, 0, 0,
              phdr->signal_dbm,
              "%d dBm",
              phdr->signal_dbm);
    }

    if (phdr->has_noise_percent) {
      proto_tree_add_uint_format_value(radio_tree, hf_wlan_radio_noise_percent, tvb, 0, 0,
              phdr->noise_percent,
              "%u%%",
              phdr->noise_percent);
    }

    if (phdr->has_noise_dbm) {
      proto_tree_add_int_format_value(radio_tree, hf_wlan_radio_noise_dbm, tvb, 0, 0,
              phdr->noise_dbm,
              "%d dBm",
              phdr->noise_dbm);
    }

    if (phdr->has_tsf_timestamp) {
      proto_tree_add_uint64(radio_tree, hf_wlan_radio_timestamp, tvb, 0, 0,
              phdr->tsf_timestamp);
    }
    if (phdr->has_aggregate_info) {
      proto_tree_add_boolean(radio_tree, hf_wlan_last_part_of_a_mpdu, tvb, 0, 0,
              (phdr->aggregate_flags & PHDR_802_11_LAST_PART_OF_A_MPDU) ?
               TRUE : FALSE);
      proto_tree_add_boolean(radio_tree, hf_wlan_a_mpdu_delim_crc_error, tvb, 0, 0,
              (phdr->aggregate_flags & PHDR_802_11_A_MPDU_DELIM_CRC_ERROR) ?
               TRUE : FALSE);
      proto_tree_add_uint(radio_tree, hf_wlan_a_mpdu_aggregate_id, tvb, 0, 0,
              phdr->aggregate_id);
    }

    if (have_data_rate) {
      gboolean assumed_short_preamble = FALSE;
      gboolean assumed_non_greenfield = FALSE;
      gboolean assumed_no_stbc = FALSE;
      gboolean assumed_no_extension_streams = FALSE;
      gboolean assumed_bcc_fec = FALSE;

        /
        int phy = phdr->phy;
        if (phy == PHDR_802_11_PHY_11G &&
          (data_rate == 1.0f || data_rate == 2.0f ||
          data_rate == 5.5f || data_rate == 11.0f ||
          data_rate == 22.0f || data_rate == 33.0f)) {
          phy = PHDR_802_11_PHY_11B;
        }

        switch (phy) {

        case PHDR_802_11_PHY_11_FHSS:
          break;

        case PHDR_802_11_PHY_11B:
          if (!has_short_preamble) {
              assumed_short_preamble = TRUE;
          }
          preamble = short_preamble ? 72 + 24 : 144 + 48;

          /
          /
          duration = (guint) ceil(preamble + frame_length * 8 / data_rate);
          break;

        case PHDR_802_11_PHY_11A:
        case PHDR_802_11_PHY_11G:
        {
          /
          /

          /
          guint bits = 16 + 8 * frame_length + 6;
          guint symbols = (guint) ceil(bits / (data_rate * 4));

          /
          preamble = 16 + 4;

          duration = preamble + symbols * 4;
          break;
        }

        case PHDR_802_11_PHY_11N:
        {
          struct ieee_802_11n *info_n = &phdr->phy_info.info_11n;
          guint bandwidth_40;

          /
          static const guint Nhtdltf[4] = {1, 2, 4, 4};
          static const guint Nhteltf[4] = {0, 1, 2, 4};
          guint Nsts, bits, Mstbc, bits_per_symbol, symbols;
          guint stbc_streams;
          guint ness;
          gboolean fec;

          /
          if (!info_n->has_mcs_index ||
            info_n->mcs_index > MAX_MCS_INDEX ||
            !info_n->has_bandwidth ||
            !info_n->has_short_gi)
              break;

          bandwidth_40 = info_n->bandwidth == PHDR_802_11_BANDWIDTH_40_MHZ;

          /

          /
          if (info_n->has_greenfield) {
            preamble = info_n->greenfield ? 24 : 32;
          } else {
            preamble = 32;
            assumed_non_greenfield = TRUE;
          }

          if (info_n->has_stbc_streams) {
            stbc_streams = info_n->stbc_streams;
          } else {
            stbc_streams = 0;
            assumed_no_stbc = TRUE;
          }

          if (info_n->has_ness) {
            ness = info_n->ness;
            if (ness >= G_N_ELEMENTS(Nhteltf)) {
                /
                break;
            }
          } else {
            ness = 0;
            assumed_no_extension_streams = TRUE;
          }

          /
          Nsts = ieee80211_ht_streams[info_n->mcs_index] + stbc_streams;
          if (Nsts == 0 || Nsts - 1 >= G_N_ELEMENTS(Nhtdltf)) {
              /
              break;
          }
          preamble += 4 * (Nhtdltf[Nsts-1] + Nhteltf[ness]);

          if (info_n->has_fec) {
            fec = info_n->fec;
          } else {
            fec = 0;
            assumed_bcc_fec = TRUE;
          }

          /
          if (fec == 0) {
            /
            bits = 8 * frame_length + 16 + ieee80211_ht_Nes[info_n->mcs_index] * 6;
            Mstbc = stbc_streams ? 2 : 1;
            bits_per_symbol = ieee80211_ht_Dbps[info_n->mcs_index] * (bandwidth_40 ? 2 : 1);
            symbols = bits / (bits_per_symbol * Mstbc);
          } else {
            /
            bits = 8 * frame_length + 16 + ieee80211_ht_Nes[info_n->mcs_index] * 6;
            Mstbc = stbc_streams ? 2 : 1;
            bits_per_symbol = ieee80211_ht_Dbps[info_n->mcs_index] * (bandwidth_40 ? 2 : 1);
            symbols = bits / (bits_per_symbol * Mstbc);
          }

          /
          if((bits % (bits_per_symbol * Mstbc)) > 0)
            symbols++;

          symbols *= Mstbc;
          duration = preamble + (symbols * (info_n->short_gi ? 36 : 40) + 5) / 10;
          break;
        }

        case PHDR_802_11_PHY_11AC:
        {
          struct ieee_802_11ac *info_ac = &phdr->phy_info.info_11ac;
          int bits, stbc;

          /
          if (info_ac->has_stbc) {
            stbc = info_ac->stbc;
          } else {
            stbc = 0;
            assumed_no_stbc = TRUE;
          }

          preamble = 32 + 4 * info_ac->nss[0] * (stbc+1);
          bits = 8 * frame_length + 16;
          duration = (guint) (preamble + bits / data_rate);
          break;
        }
      }

    if (duration) {
        proto_item *item = proto_tree_add_uint_format_value(radio_tree, hf_wlan_radio_duration, tvb, 0, 0,
                duration,
                "%d us",
                duration);
        PROTO_ITEM_SET_GENERATED(item);

        if (assumed_short_preamble)
          expert_add_info(pinfo, item, &ei_wlan_radio_assumed_short_preamble);
        if (assumed_non_greenfield)
          expert_add_info(pinfo, item, &ei_wlan_radio_assumed_non_greenfield);
        if (assumed_no_stbc)
          expert_add_info(pinfo, item, &ei_wlan_radio_assumed_no_stbc);
        if (assumed_no_extension_streams)
          expert_add_info(pinfo, item, &ei_wlan_radio_assumed_no_extension_streams);
        if (assumed_bcc_fec)
          expert_add_info(pinfo, item, &ei_wlan_radio_assumed_bcc_fec);

        if (preamble) {
          proto_tree *d_tree = proto_item_add_subtree(item, ett_wlan_radio_duration);
          proto_item *p_item = proto_tree_add_uint_format_value(d_tree, hf_wlan_radio_preamble, tvb, 0, 0,
                    preamble,
                    "%d us",
                    preamble);
            PROTO_ITEM_SET_GENERATED(p_item);
        }
      }
    }
  } /
}
