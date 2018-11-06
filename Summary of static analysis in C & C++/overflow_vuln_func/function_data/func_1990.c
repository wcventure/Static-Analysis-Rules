static void qmp_input_start_struct(Visitor *v, void **obj, const char *kind,
                                   const char *name, size_t size, Error **errp)
{
    QmpInputVisitor *qiv = to_qiv(v);
    const QObject *qobj = qmp_input_get_object(qiv, name);

    if (!qobj || qobject_type(qobj) != QTYPE_QDICT) {
        error_set(errp, QERR_INVALID_PARAMETER_TYPE, name ? name : "null",
                  "QDict");
        return;
    }

    qmp_input_push(qiv, qobj, errp);
    if (error_is_set(errp)) {
        return;
    }

    if (obj) {
        *obj = g_malloc0(size);
    }
}
