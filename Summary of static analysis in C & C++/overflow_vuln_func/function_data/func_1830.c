void
proto_reg_handoff_ndps(void)
{
	dissector_handle_t ndps_handle, ndps_tcp_handle;

	ndps_handle = create_dissector_handle(dissect_ndps_ipx, proto_ndps);
	ndps_tcp_handle = create_dissector_handle(dissect_ndps_tcp, proto_ndps);
	
	dissector_add("spx.socket", SPX_SOCKET_PA, ndps_handle);
	dissector_add("spx.socket", SPX_SOCKET_BROKER, ndps_handle);
	dissector_add("spx.socket", SPX_SOCKET_SRS, ndps_handle);
	dissector_add("spx.socket", SPX_SOCKET_ENS, ndps_handle);
	dissector_add("spx.socket", SPX_SOCKET_RMS, ndps_handle);
	dissector_add("spx.socket", SPX_SOCKET_NOTIFY_LISTENER, ndps_handle);
	dissector_add("tcp.port", TCP_PORT_PA, ndps_tcp_handle);
	dissector_add("tcp.port", TCP_PORT_BROKER, ndps_tcp_handle);
	dissector_add("tcp.port", TCP_PORT_SRS, ndps_tcp_handle);
	dissector_add("tcp.port", TCP_PORT_ENS, ndps_tcp_handle);
	dissector_add("tcp.port", TCP_PORT_RMS, ndps_tcp_handle);
	dissector_add("tcp.port", TCP_PORT_NOTIFY_LISTENER, ndps_tcp_handle);
	ndps_data_handle = find_dissector("data");
}
