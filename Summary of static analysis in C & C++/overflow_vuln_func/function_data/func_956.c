void
proto_reg_handoff_wlan(void)
{
  dissector_handle_t ieee80211_handle;
  dissector_handle_t ieee80211_radio_handle;

  /
  llc_handle = find_dissector("llc");
  ipx_handle = find_dissector("ipx");
  data_handle = find_dissector("data");

  ieee80211_handle = find_dissector("wlan");
  dissector_add("wtap_encap", WTAP_ENCAP_IEEE_802_11, ieee80211_handle);
  ieee80211_radio_handle = create_dissector_handle(dissect_ieee80211_radio,
						   proto_wlan);
  dissector_add("wtap_encap", WTAP_ENCAP_IEEE_802_11_WITH_RADIO,
		ieee80211_radio_handle);
}
