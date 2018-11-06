MigrationIncomingState *migration_incoming_state_new(QEMUFile* f)
{
    mis_current = g_malloc0(sizeof(MigrationIncomingState));
    mis_current->file = f;
    QLIST_INIT(&mis_current->loadvm_handlers);

    return mis_current;
}
