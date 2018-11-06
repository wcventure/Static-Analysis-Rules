void h223_set_mc( packet_info* pinfo, guint8 mc, h223_mux_element* me )
{
    circuit_t *circ = find_circuit( pinfo->ctype, pinfo->circuit_id, pinfo->fd->num );
    h223_vc_info* vc_info;
    DISSECTOR_ASSERT( circ );
    vc_info = circuit_get_proto_data(circ, proto_h223);
    add_h223_mux_element( &(vc_info->call_info->direction_data[pinfo->p2p_dir]), mc, me, pinfo->fd->num );
}
