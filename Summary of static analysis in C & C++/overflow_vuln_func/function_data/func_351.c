void h223_add_lc( packet_info* pinfo, guint16 lc, h223_lc_params* params )
{
    circuit_t *circ = find_circuit( pinfo->ctype, pinfo->circuit_id, pinfo->fd->num );
    h223_vc_info* vc_info;
    DISSECTOR_ASSERT( circ );
    vc_info = circuit_get_proto_data(circ, proto_h223);
    init_logical_channel( pinfo, vc_info->call_info, lc, pinfo->p2p_dir, params );
}
