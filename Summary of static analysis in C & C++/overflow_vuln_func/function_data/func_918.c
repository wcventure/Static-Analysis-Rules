static gboolean
dissect_opensafety_epl(tvbuff_t *message_tvb, packet_info *pinfo, proto_tree *tree, void *data _U_ )
{
    gboolean        result     = FALSE;
    guint8          firstByte;
    proto_tree      *epl_tree = NULL;

    if ( ! global_enable_plk )
        return result;

    /
    if ( bDissector_Called_Once_Before == FALSE )
    {
        bDissector_Called_Once_Before = TRUE;

        firstByte = ( tvb_get_guint8(message_tvb, 0) << 1 );

        /
        if ( ( firstByte != 0x02 ) && ( firstByte != 0x0A ) )
        {
            /
            epl_tree = tree;
            while ( epl_tree != NULL && epl_tree->parent != NULL )
                epl_tree = epl_tree->parent;

            result = opensafety_package_dissector("openSAFETY/Powerlink", "",
                                                  FALSE, FALSE, 0, message_tvb, pinfo, epl_tree);
        }

        bDissector_Called_Once_Before = FALSE;
    }

    return result;
}
