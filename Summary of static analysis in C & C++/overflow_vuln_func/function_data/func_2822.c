static QObject *qmp_output_first(QmpOutputVisitor *qov)
{
    QStackEntry *e = QTAILQ_LAST(&qov->stack, QStack);

    /
    if (!e) {
        return NULL;
    }

    return e->value;
}
